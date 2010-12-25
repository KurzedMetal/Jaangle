' VBScript File

Function GetProjectDir(FullName)

'VC++ doesn't provide any method for getting the path of the active project
'See the VB Script reference for more information on the VB Script functions
'used in this function

Dim proj_path
proj_path = Split(StrReverse(FullName),"\",-1,1)

Dim count
count = UBound(proj_path)

Dim full_path
full_path = ""
Dim i

for i = 1 to count
	full_path = full_path & "\" & proj_path(i)
next

GetProjectDir = StrReverse(full_path)

End Function


Sub ReplaceText(selection, count, incrementby)

'selection represents the TextSelection object
'count represents the position of the version number to be incremented
'incrementby represents a number that will be added to the existing version number

selection.WordRight dsMove, count
selection.WordRight dsExtend, 1
Dim str
str = selection.Text
str = str + incrementby

selection.Text = str

End Sub


Sub Application_BuildFinish(numError, numWarning)

'This event will be triggered after every build of a project
'You can check numError and/or numWarning to determine if you want to continue
'If numError <> 0 Then
    'exit sub
'Obtain the full path of the active project
Dim full_path
full_path = GetProjectDir(ActiveProject.FullName)

full_path = full_path & "versionno.h"

'Open the VersionNo.h file
Documents.Open full_path

'Obtain the TextSelection object
Dim selection
set selection = ActiveDocument.Selection
selection.StartOfDocument

'Increment the version information
ReplaceText selection, 9, 1
selection.LineDown
selection.StartOfLine
ReplaceText selection, 9, 1
selection.LineDown
selection.StartOfLine
ReplaceText selection, 10, 1
selection.LineDown
selection.StartOfLine
ReplaceText selection, 10, 1

ActiveDocument.Save
ActiveDocument.Close

End Sub
