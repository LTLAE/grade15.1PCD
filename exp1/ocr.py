"""
使用 pytesseract 进行 OCR 识别
需要在终端安装 tesseract-ocr
macOS 下安装 tesseract-ocr 的方法：
        brew install tesseract
        brew install tesseract-lang
"""

import os

import pytesseract
from PIL import Image
import subprocess


def ocr_from_images_in_folder(folder_path):
    # get all files in the folder
    files = os.listdir(folder_path)

    for file in files:
        # filter out images
        if file.lower().endswith(('.png', '.jpg', '.jpeg', '.tiff', '.bmp', '.gif')):
            image_path = os.path.join(folder_path, file)
            try:
                image = Image.open(image_path)
                text = pytesseract.image_to_string(image, lang='eng')
                return text
            except Exception as e:
                print(f"Failed processing {file} : {e}OCR failed.")


if __name__ == "__main__":
    path = "."  # current folder
    ocrText = ocr_from_images_in_folder(path)
    print("OCR Text: ", ocrText)
    # save OCR text to code.txt
    with open("code.txt", "w") as f:
        f.write(ocrText)
    print("OCR text saved to code.txt, Calling ./main to recognize")
    subprocess.run(["./main"])
    print("OCR text recognize finished, deleting code.txt")
    os.remove("code.txt")

# special thanks to Bianco KB Eralbrunia for the help with OCR
