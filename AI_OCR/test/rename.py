import os
import struct

def get_png_size(path):
    """
    Extrait largeur et hauteur d'un fichier PNG
    en lisant uniquement le header IHDR.
    """
    with open(path, "rb") as f:
        # Vérification signature PNG
        signature = f.read(8)
        if signature != b"\x89PNG\r\n\x1a\n":
            raise ValueError("Not a PNG file")

        # Chunk IHDR : 4 bytes (length), 4 bytes (IHDR), puis données
        length = struct.unpack(">I", f.read(4))[0]
        chunk_type = f.read(4)

        if chunk_type != b"IHDR":
            raise ValueError("IHDR chunk not found")

        # IHDR: width (4 bytes) + height (4 bytes)
        width, height = struct.unpack(">II", f.read(8))
        return width, height


def get_max_png_dimensions(folder):
    max_width = 0
    max_height = 0
    max_width_file = None
    max_height_file = None

    for filename in os.listdir(folder):
        if filename.lower().endswith(".png"):
            path = os.path.join(folder, filename)
            try:
                width, height = get_png_size(path)

                if width > max_width:
                    max_width = width
                    max_width_file = filename

                if height > max_height:
                    max_height = height
                    max_height_file = filename
            except Exception as e:
                print(f"Erreur sur {filename}: {e}")

    return max_width, max_height, max_width_file, max_height_file


# Exemple d'utilisation
if __name__ == "__main__":
    folder = "training_data/"

    max_w, max_h, file_w, file_h = get_max_png_dimensions(folder)

    print(f"Largeur max : {max_w} px (fichier : {file_w})")
    print(f"Hauteur max : {max_h} px (fichier : {file_h})")
