#!/usr/bin/env python3
import os
from PIL import Image, ImageDraw

def create_placeholder(folder, name, color, text):
    os.makedirs(folder, exist_ok=True)
    img = Image.new('RGBA', (128, 128), color=(0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    # Draw a rounded rectangle or simple box
    draw.rectangle([10, 10, 118, 118], fill=color, outline="white", width=2)
    # Note: Text requires a font, so we'll just stick to colors for simplicity 
    # unless you have a font file. Let's just use different colors.
    img.save(os.path.join(folder, f"{name}.png"))
    print(f"Created: {folder}/{name}.png")

if __name__ == "__main__":
    try:
        # Create test assets with different colors for each state
        create_placeholder("assets/idle", "1", (100, 149, 237, 255), "IDLE") # Blue
        
        create_placeholder("assets/walk", "1", (144, 238, 144, 255), "WALK1") # Green
        create_placeholder("assets/walk", "2", (50, 205, 50, 255), "WALK2")
        
        create_placeholder("assets/sleep", "1", (255, 182, 193, 255), "SLEEP1") # Pink
        create_placeholder("assets/sleep", "2", (255, 105, 180, 255), "SLEEP2")
        
        create_placeholder("assets/drag", "1", (255, 165, 0, 255), "DRAG") # Orange
        
        print("\nTest assets generated successfully!")
        print("Now you can run 'make' and './bin/litepaws' to see the colored boxes in action.")
    except ImportError:
        print("Error: You need the 'Pillow' library for this script.")
        print("Run: pip install Pillow")
