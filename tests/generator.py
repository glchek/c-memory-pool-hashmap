# это надо переделать в генератор raw-текста в latex, чтобы картинки тоже автоматически вставлять
# либо заморочиться с генерацией .docx
import pyautogui
import time


def interrupt():
    pyautogui.hotkey("ctrl", "c")


def makeComment(s):
    if s == "":
        print("", file=outputRaw)
        return None
    if s[-1] == "\n":
        print(s[:-1], file=outputRaw)
    else:
        print(s, file=outputRaw)


def setTestedFile(name):
    global directory, cmdStart
    name = name[:-1]
    cmdStart = directory + name
    interrupt()
    send("cls")
    send(cmdStart)


def pause(s):
    interrupt()
    input()
    time.sleep(2)
    send("cls")
    send(cmdStart)


def set_test(s):
    global tests_completed
    tests_completed = int(s) - 1

def setWaitTime(s):
    global waitTime
    waitTime = float(s)

# directory = '.\lab6\\'
directory = ".\\"
fadr = directory + "input.txt"
file = open(fadr, encoding="utf-8")
endStrings = ["#end", "#end\n", ""]
commandStrings = {
    "#T": makeComment,
    "#ChangeFile": setTestedFile,
    "#pause": pause,
    "#setTest": set_test,
    "#setWaitTime": setWaitTime
}
waitTime = 0


def enter(string):
    pyautogui.typewrite(string)
    time.sleep(waitTime)


def send(string):
    pyautogui.typewrite(string)
    pyautogui.press("enter")


cmdStart = directory + "lab"
string = ""
time.sleep(2)
mouseStart = (700, 800)
pyautogui.moveTo(mouseStart[0], mouseStart[1])
pyautogui.click()
tests_completed = 0
outputRaw = open("raw text.txt", "w", encoding="utf-8")
isNewTest = True
isSomethingEntered = False
isNewSession = True

print(cmdStart)
send("cls")
send(cmdStart)
while True:
    string = file.readline().replace("\\n", "\n").replace("\\t", "\t")
    print("string:\t ", list(string))
    if string[0] == "*":
        continue
    if isNewTest:
        makeComment("Тест №" + str(tests_completed + 1))
    isNewTest = False
    if string[0] == "#":
        if string.split()[0] in commandStrings.keys():
            commandStrings[string.split()[0]](
                string.removeprefix(string.split()[0] + " ")
            )
            continue
        elif string not in endStrings:
            print(string, "is treated like command but it is not")
            exit(1)
    if string == "\n" or string in endStrings:
        tests_completed += 1
        isNewTest = True
        isSomethingEntered = False
        makeComment("")
        interrupt()
        pyautogui.screenshot(
            f"{directory}{tests_completed}.png", region=(335, 80, 1585, 950)
        )
        time.sleep(1)
        if string in endStrings:
            break
        send("cls")
        send(cmdStart)
        continue

    if not isSomethingEntered:
        makeComment("Ввод: ")
    enter(string)
    makeComment(string)
    isSomethingEntered = 1

outputRaw.close()
