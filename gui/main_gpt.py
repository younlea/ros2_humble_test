import tkinter as tk
from tkinter import messagebox

# 애플리케이션의 기본 창 생성
root = tk.Tk()
root.title("간단한 GUI 예제")
root.geometry("400x300")

# 메뉴바 생성
menu_bar = tk.Menu(root)

# 파일 메뉴 생성
file_menu = tk.Menu(menu_bar, tearoff=0)
file_menu.add_command(label="열기", command=lambda: messagebox.showinfo("파일", "파일 열기 선택됨"))
file_menu.add_command(label="저장", command=lambda: messagebox.showinfo("파일", "파일 저장 선택됨"))
file_menu.add_separator()
file_menu.add_command(label="종료", command=root.quit)
menu_bar.add_cascade(label="파일", menu=file_menu)

# 도구 메뉴 생성
tool_menu = tk.Menu(menu_bar, tearoff=0)
tool_menu.add_command(label="메시지 보여주기", command=lambda: show_text("메시지가 여기에 표시됩니다."))
menu_bar.add_cascade(label="도구", menu=tool_menu)

# 메뉴바를 루트 창에 추가
root.config(menu=menu_bar)

# 텍스트 위젯 생성 및 배치
text = tk.Text(root, height=10, width=40)
text.pack(pady=20)

# 텍스트를 보여주는 함수
def show_text(msg):
    text.delete(1.0, tk.END)  # 기존 텍스트 삭제
    text.insert(tk.END, msg)  # 새로운 텍스트 삽입

# 메인 루프 실행
root.mainloop()
