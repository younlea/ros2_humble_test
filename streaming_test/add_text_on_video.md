import cv2
import datetime

def add_text_to_frame(frame, text, position, font=cv2.FONT_HERSHEY_SIMPLEX, font_scale=1, color=(0, 255, 0), thickness=2):
    """
    Adds text to the given frame at the specified position.
    """
    cv2.putText(frame, text, position, font, font_scale, color, thickness, cv2.LINE_AA)

# Open a video file or capture from webcam
cap = cv2.VideoCapture(0)  # Use 0 for webcam or replace with video file path

if not cap.isOpened():
    print("Error: Cannot open video source.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("End of video or cannot read frame.")
        break

    # Get current date and time
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    custom_text = "Your Custom Text Here"

    # Define positions for the text
    frame_height, frame_width = frame.shape[:2]
    time_position = (frame_width - 400, 50)  # Adjust x and y for top-right corner
    text_position = (frame_width - 400, 100)  # Below the time

    # Add date/time and custom text to the frame
    add_text_to_frame(frame, current_time, time_position)
    add_text_to_frame(frame, custom_text, text_position)

    # Display the frame
    cv2.imshow("Video with Text", frame)

    # Exit on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
