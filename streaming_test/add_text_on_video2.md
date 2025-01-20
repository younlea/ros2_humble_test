PyQt에서 OpenCV로 변환하지 않고 **QLabel**을 사용하여 텍스트를 추가하는 간단한 방법을 제공하겠습니다. QLabel은 텍스트를 표시하는 데 적합하며, HTML 태그를 지원하므로 두 줄로 텍스트를 표시할 수 있습니다.

---

## **코드**

아래는 QLabel을 사용하여 오른쪽 상단에 현재 시간과 사용자 지정 텍스트를 두 줄로 표시하는 코드입니다:

```python
import sys
import datetime
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel
from PyQt5.QtCore import Qt


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("PyQt QLabel Example")
        self.setGeometry(100, 100, 800, 600)

        # Create a QLabel for displaying text
        self.text_label = QLabel(self)
        self.text_label.setAlignment(Qt.AlignRight | Qt.AlignTop)  # Align top-right
        self.text_label.setStyleSheet("font-size: 16px; color: white; background-color: black;")
        self.text_label.setGeometry(self.width() - 300, 10, 280, 50)  # Position and size of the label

        # Update the text in the label
        self.update_text()

    def update_text(self):
        """Update the label with current time and custom text."""
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        custom_text = "Your Custom Text Here"
        
        # Set text with two lines
        self.text_label.setText(f"{current_time}<br>{custom_text}")
        self.text_label.setTextFormat(Qt.RichText)  # Enable HTML formatting


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
```

---

## **코드 설명**

### **1. QLabel 생성**
- QLabel을 생성하고 부모 위젯으로 `self`를 지정합니다:
  ```python
  self.text_label = QLabel(self)
  ```

### **2. 텍스트 정렬**
- 텍스트를 오른쪽 상단에 정렬하기 위해 `setAlignment` 메서드를 사용합니다:
  ```python
  self.text_label.setAlignment(Qt.AlignRight | Qt.AlignTop)
  ```

### **3. 스타일 설정**
- `setStyleSheet`를 사용하여 글꼴 크기, 색상 및 배경색을 설정합니다:
  ```python
  self.text_label.setStyleSheet("font-size: 16px; color: white; background-color: black;")
  ```

### **4. 위치 및 크기 지정**
- `setGeometry` 메서드를 사용하여 QLabel의 위치와 크기를 설정합니다:
  ```python
  self.text_label.setGeometry(self.width() - 300, 10, 280, 50)
  ```

### **5. 텍스트 업데이트**
- 현재 시간을 가져오고 사용자 지정 텍스트와 함께 두 줄로 표시합니다:
  ```python
  current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
  custom_text = "Your Custom Text Here"
  
  self.text_label.setText(f"{current_time}<br>{custom_text}")
  ```
- `setTextFormat(Qt.RichText)`를 사용하여 HTML 태그(`<br>`)를 지원하도록 설정합니다.

---

## **결과**

1. 오른쪽 상단에 두 줄의 텍스트가 표시됩니다:
   - 첫 번째 줄: 현재 시간 (예: "2025-01-20 16:00:00").
   - 두 번째 줄: 사용자 정의 텍스트 (예: "Your Custom Text Here").
2. 배경색은 검정색(`background-color: black`), 글자색은 흰색(`color: white`)으로 설정됩니다.
3. 창 크기를 조정해도 위치는 고정됩니다.

---

## **확장**

- **실시간 업데이트**:
   - 타이머(`QTimer`)를 사용하여 매초마다 시간을 업데이트할 수 있습니다:
     ```python
     from PyQt5.QtCore import QTimer

     # Add this to __init__()
     self.timer = QTimer(self)
     self.timer.timeout.connect(self.update_text)
     self.timer.start(1000)  # Update every second
     ```

- **동적 위치 조정**:
   - 창 크기가 변경될 때 라벨의 위치를 동적으로 조정하려면 `resizeEvent` 메서드를 재정의하세요:
     ```python
     def resizeEvent(self, event):
         self.text_label.move(self.width() - 300, 10)
         super().resizeEvent(event)
     ```

이 코드는 OpenCV 변환 없이 PyQt만으로 간단히 텍스트를 추가하는 방법을 제공합니다!

출처
[1] Python PyQt text updater application - w3resource https://www.w3resource.com/python-exercises/pyqt/python-pyqt-connecting-signals-to-slots-exercise-3.php
[2] How to display opencv video in pyqt apps - GitHub Gist https://gist.github.com/docPhil99/ca4da12c9d6f29b9cea137b617c7b8b1?permalink_comment_id=4360451
[3] Overlay text over a specific widget (transparent labels) in pyqt4 https://stackoverflow.com/questions/14042060/overlay-text-over-a-specific-widget-transparent-labels-in-pyqt4
[4] PyQt5 – How to change text of pre-existing label | setText method https://www.geeksforgeeks.org/pyqt5-how-to-change-text-of-pre-existing-label-settext-method/
[5] Qt 4.7: QLabel Class Reference - Developpez.com https://qt.developpez.com/doc/4.7/qlabel
[6] PyQt label | Learn Python PyQt https://pythonpyqt.com/pyqt-label/
[7] Use QLineEdit to Add Text Input Fields to Your PyQt/PySide GUIs https://www.pythonguis.com/docs/qlineedit/
[8] Adding images to PyQt5 applications, using QLabel and QPixmap https://www.pythonguis.com/faq/adding-images-to-pyqt5-applications/
[9] How to Add Text and Image in PyQt5 Label - Codeloop https://codeloop.org/how-to-add-text-and-image-in-pyqt5-label/
[10] Add text in Qlabel - python - Stack Overflow https://stackoverflow.com/questions/67305420/add-text-in-qlabel
[11] How to add a new line at a particular position in QLabel - Qt Forum https://forum.qt.io/topic/40297/how-to-add-a-new-line-at-a-particular-position-in-qlabel
[12] How to embed text to pixmap in QLabel? - Qt Centre Forum https://www.qtcentre.org/threads/69015-How-to-embed-text-to-pixmap-in-QLabel
[13] How to create Label widget in PyQt5 ? - GeeksforGeeks https://www.geeksforgeeks.org/how-to-create-label-widget-in-pyqt5/
[14] PyQt5 Tutorial #2 - Working with QLabel - YouTube https://www.youtube.com/watch?v=3lieu9pFbIQ
[15] Displaying a standard icon and text in QLabel - pyqt - Stack Overflow https://stackoverflow.com/questions/10533838/displaying-a-standard-icon-and-text-in-qlabel
[16] Pyqt6 QLabel Widget - Displaying Text on your Window - YouTube https://www.youtube.com/watch?v=FoD7MOm4KY4
[17] PySide6.QtWidgets.QLabel - Qt for Python https://doc.qt.io/qtforpython-6/PySide6/QtWidgets/QLabel.html
[18] qlabel not changing dynamically - Qt Forum https://forum.qt.io/topic/122469/qlabel-not-changing-dynamically
[19] 02) QLabel - PyQt5 Tutorial - 파이썬으로 만드는 나만의 GUI 프로그램 https://wikidocs.net/21939
[20] Thread: geting QLabel text ontop of other QLabel displaying image https://www.qtcentre.org/threads/57274-geting-QLabel-text-ontop-of-other-QLabel-displaying-image
[21] Label text overlap another label text when both the labels ... - Qt Forum https://forum.qt.io/topic/36548/label-text-overlap-another-label-text-when-both-the-labels-at-the-same-position
[22] PyQt5 – How to change font and size of Label text ? - GeeksforGeeks https://www.geeksforgeeks.org/pyqt5-how-to-change-font-and-size-of-label-text/
