' Attribute VB_Name = "NIDAQmx"
<Assembly: VB_Name(, "NIDAQmx")> ' not sure if this is correct syntax AWT [5/14/2018]

Public Sub DAQmxErrChk(errorCode As Long)
'
'   Utility function to handle errors by recording the DAQmx error code
'   and message.
'
    Dim errorString As String
    Dim bufferSize As Long
    Dim status As Long
    If (errorCode < 0) Then
        ' Find out the error message length.
        bufferSize = DAQmxGetErrorString(errorCode, 0, 0)
        ' Allocate enough space in the string.
        errorString = String$(bufferSize, 0)
        ' Get the actual error message.
        status = DAQmxGetErrorString(errorCode, errorString, bufferSize)
        ' Trim it to the actual length, and display the message
        errorString = Left(errorString, InStr(errorString, Chr$(0)))
        Err.Raise(errorCode, , errorString)
    End If

    
End Sub



