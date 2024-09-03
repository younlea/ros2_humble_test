import tkinter as tk
from tkinter import messagebox

def show_message():
    messagebox.showinfo("메시지", "버튼이 클릭되었습니다!")

# 메인 윈도우 생성
root = tk.Tk()
root.title("간단한 GUI 예제")
root.geometry("300x200")

# 메뉴 생성
menubar = tk.Menu(root)
root.config(menu=menubar)

filemenu = tk.Menu(menubar, tearoff=0)
menubar.add_cascade(label="파일", menu=filemenu)
filemenu.add_command(label="종료", command=root.quit)

# 버튼 추가
button = tk.Button(root, text="클릭하세요", command=show_message)
button.pack(pady=20)

# 텍스트 레이블 추가
label = tk.Label(root, text="이것은 간단한 GUI 예제입니다.")
label.pack(pady=10)

# GUI 실행
root.mainloop()
