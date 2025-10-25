import subprocess
import sys
import shutil
import os

def get_picotool_path():
    # Try to find picotool in PATH
    picotool_path = shutil.which("picotool")
    
    if picotool_path:
        return picotool_path

    # Prompt user if not found
    print("picotool not found in system PATH.")
    user_path = input("Please enter full path to picotool executable: ").strip()

    # Verify the file exists and is executable
    if not os.path.isfile(user_path) or not os.access(user_path, os.X_OK):
        print("Invalid picotool path or not executable.")
        sys.exit(1)
    
    return user_path

def get_name_from_args_or_input():
    if len(sys.argv) > 1:
        try:
            name = sys.argv[1]

            return serial
        except ValueError:
            print("needs a name.")
            sys.exit(1)
    else:
        # Fallback to prompt
        try:
            serial = input("Enter new name ")
            return serial
        except ValueError:
            print("Invalid input. Must be an integer between 0 and 255.")
            sys.exit(1)

def main():
    serial = get_name_from_args_or_input()

    # Load the original .bin file
    input_filename = "config_flash.bin"
    output_filename = "m_config_flash.bin"

    try:
        with open(input_filename, "rb") as f:
            data = bytearray(f.read())
    except FileNotFoundError:
        print(f"Error: {input_filename} not found.")
        sys.exit(1)

    # Modify the first byte
    data[0] = len(serial)+1


    # Save the modified file
    with open(output_filename, "wb") as f:
        f.write(data)
    print(f"Modified file saved as: {output_filename}")

    # Get picotool path (from PATH or user)
    picotool = get_picotool_path()

    # Run the picotool command and forward output
    try:
        print(f"Running: {picotool} load -o 101FF000 {output_filename}")
        subprocess.run(
            [picotool, "load", "-o", "101FF000", output_filename],
            check=True,
            stdout=sys.stdout,
            stderr=sys.stderr
        )
    except subprocess.CalledProcessError as e:
        print(f"picotool failed with exit code {e.returncode}")
        sys.exit(1)

if __name__ == "__main__":
    main()
