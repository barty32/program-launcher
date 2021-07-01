# Program Launcher
Since I have many programs on my USB flash drive, I needed a solution to simply and quickly launch those programs. Browsing through my flash disk is slow and quite annoying, and I wanted to open them quickly from one place. This can't be done using shortcuts, because they need absolute paths. So I created Program Launcher.

It can be also used to group Desktop shortcuts. If you have a lot of them, you can move those less used to Program Launcher.


This is simple program launcher for Windows, intended for use on USB flash drives, where its drive letters is different on each computer. Main goal of this application is its portability. However, it can be run only on Windows.


# How to use
When you first open Program Launcher, you will find that it's empty.

Start by adding a new category (tab) by clicking the 'Category' menu item, and then choosing 'Add category...' item, or you could use Ctrl+G keyboard shortcut.

Then enter name of the new category into the newly created dialog, it can be for example 'Programs' or 'Games'.

**Note:** category names must not contain semicolon ';' characters and must not be any of the following keywords: 'general', 'appereance' and 'categories'.

Now you should see the newly created tab in top of main window.

Let's continue by adding a program entry (or button). Later you will be able to open a program using this entry (it works very similarly to shortcuts).

Now you have multiple ways how to add a program entry:
1. On the 'Button' menu, clicking 'Add new...'
2. Right-clicking anywhere in window's area and selecting 'Add Button...'
3. Using Ctrl+N shortcut
4. Manually editing configuration file (advanced, @see Editing .ini file)


- Display name: this name is displayed under icon 
- Path to file: specifies path to the executable (you should use the 'Browse' button)

- Path to 64-bit version of the program: if the target program also has its 64-bit version, then tick the checkbox and specify path to 64-bit executable in the text box
- Path to icon: 
- Absolute path: You should enable this if the target program is on different drive than Program Launcher (for example 'D:\Utils\Program Launcher\ProgramLauncher.exe' and 'C:\Program Files\HxD\HxD.exe')
- Add to category: here select in which category the new entry should be added
- Run as Administrator: run the target program as Admin



**Tip:** If you use the 'Browse' button to find the program's executable, other fields of this dialog will be auto-filled when possible


# Options
Most of the options are self-explanatory, just try them out to see what they do. If you mess something up, you can always Reset to defaults, it will not delete any categories or entries.



# Editing ProgramLauncher.ini file manually

**Sections:**

[general]
- ShowAppNames:
specifies where to show entry labels, 0=don't show, 1=in tooltip, 2=under icon & in tooltip
- CloseAfterLaunch: if 1, Program Launcher will exit after successful launching of the program
- Language: not implemented
- WindowWidth, WindowHeight: window dimensions
- UseIconCaching: determines whether to use icon caching, see [Icon caching]

[appereance]
- IconSize: size of icons in pixels, should be one of standard icon sizes
- IconSpacingHorizontal, IconSpacingVertical: icon spacings, in pixels

[categories]
- Categories: a semicolon delimited list of all categories (tabs)

Example:
```
[categories]
Categories=Programs;Games;Utils
```

For each name in the Categories key there should be a section with same name containing its entries

```
[categories]
Categories=Programs;Games;Utils

[Programs]
#program entries

[Games]
#game entries

[Utils]
#utils entries
```
Each category is one section in .ini file and each section can contain any amount of entries. Entries are saved in zero-based index. Each entry can contain these values:
- `Path` path to the target program
- `Path64` path to 64-bit executable of the program (optional, contains '0' if not used)
- `PathIcon` path to icon (in most cases same as Path)
- `IconIndex` index of the icon in file specified by PathIcon
- `Name` displayed name
- `Admin` run as admin
- `AbsolutePaths` use absolute paths

**Example:**
```
[categories]
Categories=Programs

[Programs]
#entry for first program
0.Path=\Programs\7-Zip\32-bit\7zFM.exe
0.Path64=\Programs\7-Zip\7zFM.exe
0.PathIcon=\Programs\7-Zip\7zFM.exe
0.IconIndex=0
0.Name=7-Zip
0.Admin=0
0.AbsolutePaths=0

#entry for second program
1.Path=\Programs\Advanced Uninstaller\uninstaller.exe
1.Path64=0
1.PathIcon=\Programs\Advanced Uninstaller\uninstaller.exe
1.IconIndex=4
1.Name=Advanced Uninstaller
1.Admin=1
1.AbsolutePaths=0

...

#entry for nth program
n.Path=...
n.Path64=...
...

```

If you don't use absolute path, write paths without drive letter:

```
0.Path=\Programs\HxD\HxD32.exe
0.Path64=\Programs\HxD\HxD64.exe
0.PathIcon=\Programs\HxD\HxD32.exe
0.AbsolutePaths=0
```

but if you do, you must specify it in all paths:
```
0.Path=A:\Programs\HxD\HxD32.exe
0.Path64=A:\Programs\HxD\HxD64.exe
0.PathIcon=A:\Programs\HxD\HxD32.exe
0.AbsolutePaths=1
```

**Note:**
These non-absolute paths are not relative paths! They are full paths with just removed drive letter (they always go from root folder of the drive). Also, the '\' in the beginning of the path is neccessary.


# Icon caching



# Installation
Just copy the executable to your flash disk (or...)



An open source firmware for DSO-138 Oscilloscope. 
![Photo](https://github.com/ardyesp/DLO-138/blob/master/pics/pic4.png)




