# infinite_loop.py

import time

def main():
    while True:
        print("This is an infinite loop. Press Ctrl+C to stop.")
        time.sleep(1)  # Sleep for 1 second to avoid flooding the output

if __name__ == "__main__":
    main()
