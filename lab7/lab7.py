Python Code: 

import serial 
import sqlite3  #database lib
import tkinter as tk 
from tkinter import ttk

# ================= DATABASE =================

conn = sqlite3.connect("rfid_database.db") #creates a database file, if not existed it creates it
cursor = conn.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS tags (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT UNIQUE,
    scans INTEGER
)
""")

conn.commit() #writes chnges to database file

# ================= SERIAL =================

arduino = serial.Serial('/dev/cu.usbmodem1201', 9600)

# CHANGE COM PORT IF NEEDED

# ================= GUI =================

root = tk.Tk() #creates application window
root.title("RFID Database")

root.geometry("500x300")

tree = ttk.Treeview(root)

tree["columns"] = ("ID", "UID", "Scans")

tree.column("#0", width=0, stretch=tk.NO)

tree.column("ID", anchor=tk.CENTER, width=80)
tree.column("UID", anchor=tk.CENTER, width=250)
tree.column("Scans", anchor=tk.CENTER, width=100)

tree.heading("ID", text="ID")
tree.heading("UID", text="UID")
tree.heading("Scans", text="Scans")

tree.pack(fill="both", expand=True)

# ================= FUNCTIONS =================

def refresh_table():

    for row in tree.get_children():
        tree.delete(row)

    cursor.execute("SELECT * FROM tags")

    rows = cursor.fetchall()

    for row in rows:
        tree.insert("", tk.END, values=row)

def update_database(uid):

    cursor.execute(
        "SELECT * FROM tags WHERE uid=?",
        (uid,)
    )

    result = cursor.fetchone()

    if result:

        scans = result[2] + 1

        cursor.execute(
            "UPDATE tags SET scans=? WHERE uid=?",
            (scans, uid)
        )

    else:

        cursor.execute(
            "INSERT INTO tags(uid, scans) VALUES(?, ?)",
            (uid, 1)
        )

    conn.commit()

    refresh_table()

def read_serial():

    if arduino.in_waiting:

        line = arduino.readline().decode().strip()

        print(line)

        if line.startswith("TAG:"):

            uid = line.replace("TAG:", "")

            update_database(uid)

    root.after(100, read_serial)

refresh_table()

read_serial()

root.mainloop()
