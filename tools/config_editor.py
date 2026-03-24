#!/usr/bin/env python3
import tkinter as tk
from tkinter import messagebox, filedialog
import os

CONFIG_PATH = os.path.join(os.path.dirname(__file__), "../config/settings.ini")

class ConfigEditor(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("LitePaws Configuration")
        self.geometry("400x300")
        
        self.settings = {}
        self.entries = {}
        
        self.load_config()
        self.create_widgets()

    def load_config(self):
        if not os.path.exists(CONFIG_PATH):
            return

        with open(CONFIG_PATH, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    key, value = line.split("=", 1)
                    self.settings[key.strip()] = value.strip()

    def save_config(self):
        try:
            with open(CONFIG_PATH, "w") as f:
                f.write("# LitePaws Configuration File\n\n")
                for key, entry in self.entries.items():
                    val = entry.get()
                    f.write(f"{key}={val}\n")
            messagebox.showinfo("Success", "Configuration saved!")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save: {e}")

    def create_widgets(self):
        row = 0
        
        # Texture Path
        tk.Label(self, text="Texture Path:").grid(row=row, column=0, sticky="w", padx=10, pady=5)
        path_entry = tk.Entry(self, width=30)
        path_entry.grid(row=row, column=1, padx=5)
        path_entry.insert(0, self.settings.get("texture_path", "assets/pet.png"))
        self.entries["texture_path"] = path_entry
        tk.Button(self, text="Browse", command=lambda: self.browse_file(path_entry)).grid(row=row, column=2, padx=5)
        row += 1

        # Width
        tk.Label(self, text="Window Width:").grid(row=row, column=0, sticky="w", padx=10, pady=5)
        w_entry = tk.Entry(self)
        w_entry.grid(row=row, column=1, sticky="w", padx=5)
        w_entry.insert(0, self.settings.get("width", "128"))
        self.entries["width"] = w_entry
        row += 1

        # Height
        tk.Label(self, text="Window Height:").grid(row=row, column=0, sticky="w", padx=10, pady=5)
        h_entry = tk.Entry(self)
        h_entry.grid(row=row, column=1, sticky="w", padx=5)
        h_entry.insert(0, self.settings.get("height", "128"))
        self.entries["height"] = h_entry
        row += 1

        # Speed
        tk.Label(self, text="Speed:").grid(row=row, column=0, sticky="w", padx=10, pady=5)
        s_entry = tk.Entry(self)
        s_entry.grid(row=row, column=1, sticky="w", padx=5)
        s_entry.insert(0, self.settings.get("speed", "50"))
        self.entries["speed"] = s_entry
        row += 1

        # Buttons
        tk.Button(self, text="Save", command=self.save_config, bg="#dddddd").grid(row=row, column=1, pady=20)

    def browse_file(self, entry):
        filename = filedialog.askopenfilename(filetypes=[("PNG Images", "*.png"), ("All Files", "*.*")])
        if filename:
            # Make relative if possible
            try:
                rel = os.path.relpath(filename, os.path.join(os.path.dirname(__file__), ".."))
                entry.delete(0, tk.END)
                entry.insert(0, rel)
            except:
                entry.delete(0, tk.END)
                entry.insert(0, filename)

if __name__ == "__main__":
    app = ConfigEditor()
    app.mainloop()
