import os

def collect_file_data(root_dir, output_file):
    with open(output_file, 'w', encoding='utf-8') as out:
        for dirpath, _, filenames in os.walk(root_dir):
            for filename in filenames:
                filepath = os.path.join(dirpath, filename)
                try:
                    with open(filepath, 'r', encoding='utf-8') as f:
                        content = f.read()
                    out.write(f"{filepath}-------------------------------------------------\n")
                    out.write(content)
                    out.write("\n\n")  # Separate entries by two newlines
                except Exception as e:
                    print(f"Failed to read {filepath}: {e}")

# === Usage ===
root_directory = 'virtus'
output_filepath = 'output.txt'

collect_file_data(root_directory, output_filepath)
