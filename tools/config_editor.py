#!/usr/bin/env python3
import tkinter as tk
from tkinter import messagebox, ttk
import os

CONFIG_PATH = os.path.join(os.path.dirname(__file__), "../config/settings.ini")

class ConfigEditor(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("LitePaws Pro Config")
        self.geometry("480x700")
        
        self.settings = {}
        self.vars = {}
        
        self.load_config()
        self.create_widgets()

    def load_config(self):
        if not os.path.exists(CONFIG_PATH):
            return
        with open(CONFIG_PATH, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"): continue
                if "=" in line:
                    key, value = line.split("=", 1)
                    self.settings[key.strip()] = value.strip()

    def save_config(self):
        try:
            with open(CONFIG_PATH, "w") as f:
                f.write("# LitePaws Configuration File\n\n")
                f.write("# Window Settings\n")
                f.write(f"width={self.vars['width'].get()}\n")
                f.write(f"height={self.vars['height'].get()}\n\n")
                
                f.write("# Behavior Toggles\n")
                for k in ['can_walk', 'can_sleep', 'can_drag', 'can_click']:
                    f.write(f"{k}={1 if self.vars[k].get() else 0}\n")
                
                f.write("\n# Movement Settings\n")
                f.write(f"walk_speed={self.vars['walk_speed'].get()}\n")
                f.write(f"gravity={self.vars['gravity'].get()}\n")
                
                f.write("\n# Animation Settings\n")
                f.write(f"frame_duration={self.vars['frame_duration'].get()}\n")
                
                f.write("\n# Animation Frame Counts\n")
                for k in ['idle_frames', 'walk_frames', 'sleep_frames', 'drag_frames']:
                    f.write(f"{k}={self.vars[k].get()}\n")
                    
            messagebox.showinfo("Success", "Configuration saved!")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save: {e}")

    def create_widgets(self):
        main_frame = ttk.Frame(self, padding="20")
        main_frame.pack(fill=tk.BOTH, expand=True)

        ttk.Label(main_frame, text="LitePaws Configuration", font=("Helvetica", 16, "bold")).pack(pady=10)

        def add_entry(label, key, default="128"):
            frame = ttk.Frame(main_frame)
            frame.pack(fill=tk.X, pady=3)
            ttk.Label(frame, text=label).pack(side=tk.LEFT)
            var = tk.StringVar(value=self.settings.get(key, default))
            entry = ttk.Entry(frame, textvariable=var, width=12)
            entry.pack(side=tk.RIGHT)
            self.vars[key] = var

        def add_toggle(label, key):
            frame = ttk.Frame(main_frame)
            frame.pack(fill=tk.X, pady=3)
            ttk.Label(frame, text=label).pack(side=tk.LEFT)
            val = int(self.settings.get(key, "1"))
            var = tk.BooleanVar(value=(val == 1))
            cb = ttk.Checkbutton(frame, variable=var)
            cb.pack(side=tk.RIGHT)
            self.vars[key] = var

        ttk.Label(main_frame, text="--- Window Size ---", font=("Helvetica", 10, "italic")).pack(pady=5)
        add_entry("Width (px):", "width")
        add_entry("Height (px):", "height")

        ttk.Label(main_frame, text="--- Behaviors ---", font=("Helvetica", 10, "italic")).pack(pady=5)
        add_toggle("Allow Walking:", "can_walk")
        add_toggle("Allow Sleeping:", "can_sleep")
        add_toggle("Allow Dragging:", "can_drag")
        add_toggle("Allow Clicking:", "can_click")

        ttk.Label(main_frame, text="--- Movement & Physics ---", font=("Helvetica", 10, "italic")).pack(pady=5)
        add_entry("Walk Speed:", "walk_speed", "60.0")
        add_entry("Gravity Force:", "gravity", "500.0")

        ttk.Label(main_frame, text="--- Animation Speed ---", font=("Helvetica", 10, "italic")).pack(pady=5)
        add_entry("Seconds per Frame:", "frame_duration", "0.5")

        ttk.Label(main_frame, text="--- Frame Counts ---", font=("Helvetica", 10, "italic")).pack(pady=5)
        add_entry("Idle frames (assets/idle/):", "idle_frames", "1")
        add_entry("Walk frames (assets/walk/):", "walk_frames", "1")
        add_entry("Sleep frames (assets/sleep/):", "sleep_frames", "1")
        add_entry("Drag frames (assets/drag/):", "drag_frames", "1")

        ttk.Button(main_frame, text="SAVE CONFIGURATION", command=self.save_config).pack(pady=20)
        
        ttk.Label(main_frame, text="Place frames as 1.png, 2.png... in assets/<state>/", font=("Helvetica", 8)).pack()

if __name__ == "__main__":
    app = ConfigEditor()
    app.mainloop()
