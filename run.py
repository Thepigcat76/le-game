import subprocess
import sys
import os

script_path = os.path.abspath(sys.argv[0])

# Get the directory part of the path
script_dir = os.path.dirname(script_path)

subprocess.run([f"{script_dir}/cozy-wrath.exe"])