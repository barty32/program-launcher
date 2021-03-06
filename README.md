# Program Launcher
Since I have many programs on my USB flash drive, I needed a solution to simply and quickly launch those programs. Browsing through my flash disk is slow and quite annoying, and I wanted to open them quickly from one place. This can't be done using shortcuts, because they need absolute paths. So I created Program Launcher.

It can be also used to group Desktop shortcuts. If you have a lot of them (like I do), you can move those less used to Program Launcher.

![Photo](https://github.com/barty32/program-launcher/blob/dev/pics/window.png)

This is simple program launcher for Windows, intended for use on USB flash drives, where its drive letters is different on each computer. Main goal of this application is its portability. However, it can be run only on Windows.


# How to use
When you open Program Launcher first time, you will find that it's empty.

![Photo](https://github.com/barty32/program-launcher/blob/dev/pics/start.png)

Start by adding a new category (tab) by clicking the 'Category' menu item, and then choosing 'Add category...' item, or use Ctrl+G keyboard shortcut. Then enter name of the new category into the newly created dialog, it can be for example `Programs` or `Games`.

**Note:** category names must not contain semicolon `;` characters and must not be any of the following keywords: `general`, `appereance` and `categories`.

Now you should see the newly created tab in upper part of the main window's client area.

![Photo2](https://github.com/barty32/program-launcher/blob/dev/pics/pic4.png)

Let's continue by adding a program entry (or button). Later you will be able to open a program using this entry (it works very similarly to shortcuts).

You have multiple options to add a program entry:
1. On the 'Button' menu, click 'Add new...'
2. Right-click anywhere in window's area and select 'Add Button...'
3. Use Ctrl+N shortcut
4. Drop a file directly inside Program Launcher's window (since v2.2)
5. Manually edit configuration file (advanced, @see Editing .ini file)

In the following dialog there are many options, but most of them are auto-filled

![photo3](https://github.com/barty32/program-launcher/blob/dev/pics/add.png)

- **Display name:** this name is displayed under icon 
- **Path to file:** specifies path to the executable (you should use the 'Browse' button)
- **Path to 64-bit version of the program:** if the target program has also its 64-bit version, then tick the checkbox and specify path to 64-bit executable in the text box
- **Path to icon:** in most cases same as 'Path to file', here you can change it to any icon
- **Absolute path:** You should enable this if the target program is on different drive than Program Launcher (for example 'D:\Utils\Program Launcher\ProgramLauncher.exe' and 'C:\Program Files\HxD\HxD.exe')
- **Add to category:** here select in which category the new entry should be added
- **Run as Administrator:** run the target program with elevated privileges
- **Icon preview:** if path specified in 'Path to icon' is valid, then an icon preview is shown

**Tip:** If you use the 'Browse' button to find the program's executable, corresponding fields of this dialog will be auto-filled when possible

If for some reason items or icons aren't displayed correctly, you can refresh the view by pressing the F5 key

# Options
Most of the options are self-explanatory, just try them out to see what they do. If you mess something up, you can always Reset to defaults, it won't delete any categories or entries.

![options](https://github.com/barty32/program-launcher/blob/dev/pics/options.png)

# Editing ProgramLauncher.ini file manually

**Sections:**

`[general]`
- `ShowAppNames`:
specifies where to show entry labels, 0=don't show, 1=in tooltip, 2=under icon & in tooltip
- `CloseAfterLaunch`: if 1, Program Launcher will exit after successful launching of the program
- `Language`: not implemented yet
- `WindowWidth`, `WindowHeight`: window dimensions
- `UseIconCaching`: determines whether to use icon caching, see [Icon caching]

Since version 2.2 this section contains three more keys:

- `ListViewMode`: last list view state 0=large icons, 1=details, 2=small icons, 3=list
- `ColumnWidths`, `ColumnOrder`: widths and order of columns in details mode

`[appereance]`
- `IconSize`: size of icons in pixels, it's better to use one of standard icon sizes (16, 24, 32, 48, 64, 128 or 256)
- `IconSpacingHorizontal`, `IconSpacingVertical`: icon spacing, in pixels

`[categories]`
- `Categories`: a semicolon delimited list of all categories (tabs)

**Example:**
```
[categories]
Categories=Programs;Games;Utils
```

For each name in the `Categories` key there should be a section with same name containing its entries

```
[categories]
Categories=Programs;Games;Utils

[Programs]
;program entries

[Games]
;game entries

[Utils]
;utils entries
```
Each category is one section in .ini file and each section can contain any amount of entries. Entries are saved in groups with zero-based index. Each entry can contain these values:
- `Path` path to the target program
- `Path64` path to 64-bit executable of the program (optional, in versions before 2.2 contains `0` if not used)
- `PathIcon` path to icon, since v2.2 it can be `$path`, that means it is same as the path
- `IconIndex` index of the icon in file specified by `PathIcon`
- `Name` displayed name
- `Admin` run as admin
- `AbsolutePaths` use paths with fixed drive letter

**Example:**
```
[categories]
Categories=Programs

[Programs]
;entry for first program
0.Path=*:\Programs\7-Zip\32-bit\7zFM.exe
0.Path64=*:\Programs\7-Zip\7zFM.exe
0.PathIcon=*:\Programs\7-Zip\7zFM.exe
0.IconIndex=0
0.Name=7-Zip
0.Admin=0
0.AbsolutePaths=0

;entry for second program
1.Path=*:\Programs\Advanced Uninstaller\uninstaller.exe
1.Path64=0
1.PathIcon=*:\Programs\Advanced Uninstaller\uninstaller.exe
1.IconIndex=4
1.Name=Advanced Uninstaller
1.Admin=1
1.AbsolutePaths=0

...

;entry for nth program
n.Path=...
n.Path64=...
...

```

Paths with variable drive letters are written as `*:\`, where at runtime the asterisk will be replaced with corresponding drive letter (because flash drives can have any drive letter on another machine).  

```
0.Path=*:\Programs\HxD\HxD32.exe
0.Path64=*:\Programs\HxD\HxD64.exe
0.PathIcon=*:\Programs\HxD\HxD32.exe
0.AbsolutePaths=0
```

or

```
0.Path=A:\Programs\HxD\HxD32.exe
0.Path64=A:\Programs\HxD\HxD64.exe
0.PathIcon=A:\Programs\HxD\HxD32.exe
0.AbsolutePaths=1
```

**Note:**
All paths in one group must be either with fixed drive letter (if `AbsolutePaths` is 1) or without fixed letter (`*:\`).
<!--**Note:**
These non-absolute paths are not relative paths! They are full paths with just removed drive letter (they always go from root folder of the drive). Also, the `\` in the beginning of the path is neccessary.-->


# Icon caching



# Installation
Just copy the executable (and possibly the configuration file) to your flash disk or any directory with write access (otherwise saving configuration file will not work properly)


# License (MIT)

MIT License

Copyright (c) 2021 barty12

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.







