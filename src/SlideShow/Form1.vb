'--------------------------------------------------------------------------
' DFTContextMenuHandler - a windows shell extension
' 
' This is a shell extension for windows that adds a file tools submenu to 
' the windows explorer context menu for folders and files in general.
'
' Version 1.01
' Copyright (c) 2004-2008  thomas694 (@GH 0CFD61744DA1A21C)
'     first version (for windows 32bit)
' Version 1.02
' Copyright (c) 2015  thomas694
'     changed to 64bit
' Version 1.03
' Copyright (c) 2017  thomas694
'     added option for emptying files
'     fixed menu ids for newer windows version
' Version 1.0.4  (c) 2020  thomas694
'     release version
' Version 1.5.0  (c) 2022  thomas694
'     copied logic code into a VS2019 ATL template,
'     using CAtlDllModuleT instead of CComModule
'
' This project/file is part of DFTContextMenuHandler.
'
' DFTContextMenuHandler is free software: you can redistribute it and/or modify
' it under the terms of the GNU General Public License as published by
' the Free Software Foundation, either version 3 of the License, or
' (at your option) any later version.
'
' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.
'
' You should have received a copy of the GNU General Public License
' along with this program.  If not, see <http://www.gnu.org/licenses/>.
'--------------------------------------------------------------------------
Imports System.IO

Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents PictureBox1 As System.Windows.Forms.PictureBox
    Friend WithEvents FolderBrowserDialog1 As System.Windows.Forms.FolderBrowserDialog
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.PictureBox1 = New System.Windows.Forms.PictureBox
        Me.FolderBrowserDialog1 = New System.Windows.Forms.FolderBrowserDialog
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.SuspendLayout()
        '
        'PictureBox1
        '
        Me.PictureBox1.BackColor = System.Drawing.Color.Black
        Me.PictureBox1.Location = New System.Drawing.Point(0, 0)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(848, 589)
        Me.PictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage
        Me.PictureBox1.TabIndex = 0
        Me.PictureBox1.TabStop = False
        '
        'FolderBrowserDialog1
        '
        Me.FolderBrowserDialog1.ShowNewFolderButton = False
        '
        'Timer1
        '
        Me.Timer1.Interval = 1000
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(848, 589)
        Me.Controls.Add(Me.PictureBox1)
        Me.MinimumSize = New System.Drawing.Size(50, 50)
        Me.Name = "Form1"
        Me.Text = "SlideShow"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private m_bStarted As Boolean
    Private m_bLoading As Boolean
    Private m_bMustRecalculate As Boolean
    Private m_bSkipEvents As Boolean
    Private lDuration As Long = 3
    Private MyImage As System.Drawing.Image
    Private MyImageVisible As System.Drawing.Image
    Private m_alPics As String()
    Private m_lIndex As Long
    Private m_lIndexF As Long
    Private m_lPathCountIndex As Long
    Private m_lMaxF As Long
    Private m_lMax As Long
    Private m_graphics As Graphics
    Private m_lX As Long
    Private m_lY As Long
    Private m_lWidth As Long
    Private m_lHeight As Long
    Private m_lX2 As Long
    Private m_lY2 As Long
    Private m_lWidth2 As Long
    Private m_lHeight2 As Long
    Private m_BlackBrush As System.Drawing.Brush
    Private m_lWindowWidth As Long
    Private m_lWindowHeight As Long

    Private m_strPreviousPath As String = String.Empty
    Public htPathCounts As New Hashtable

    Private Sub SetCaption(ByVal sFilename As String)

        Me.Text = "SlideShow (" & lDuration & "s, " & m_lIndexF & "/" & m_lMaxF & ", " & (m_lIndex + 1) & "/" & m_lMax & ") [" & m_lPathCountIndex & "/" & htPathCounts.Count & "] " & sFilename

    End Sub

    Private Sub Form1_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        If e.KeyData = Keys.Up Then
            lDuration += 1
            'refresh title bar
            SetCaption(m_alPics(m_lIndex - 1))
            Timer1.Enabled = False
            Timer1.Interval = lDuration * 1000
            Timer1.Enabled = True
        ElseIf e.KeyData = Keys.Down Then
            If lDuration > 1 Then
                lDuration -= 1
                'refresh title bar
                SetCaption(m_alPics(m_lIndex - 1))
                Timer1.Enabled = False
                Timer1.Interval = lDuration * 1000
                Timer1.Enabled = True
            End If
        ElseIf e.KeyData = Keys.P Then
            Timer1.Enabled = Not Timer1.Enabled
        ElseIf e.KeyData = Keys.F Then
            'skip rest of folder
            Timer1.Enabled = False
            Dim strPath As String = Path.GetDirectoryName(m_alPics(m_lIndex - 1))
            Dim i As Long = m_lIndex
            While i < m_lMax AndAlso strPath = Path.GetDirectoryName(m_alPics(i))
                i += 1
            End While
            'adjust counters
            m_lIndex = i
            m_lIndexF = 1
            m_strPreviousPath = Path.GetDirectoryName(m_alPics(i))
            m_lMaxF = htPathCounts(m_strPreviousPath)
            m_lPathCountIndex += 1
            LoadAndCalculateImage()
            Timer1.Enabled = True
        ElseIf e.KeyData = Keys.Escape Then
            If Timer1.Enabled Then
                Timer1.Enabled = False
            Else
                Me.Close()
            End If
        ElseIf e.KeyData = Keys.Space Then
            Debug.WriteLine("switching title bar")
            m_bSkipEvents = True
            If Me.FormBorderStyle = FormBorderStyle.None Then
                Me.FormBorderStyle = FormBorderStyle.Sizable
            Else
                Me.FormBorderStyle = FormBorderStyle.None
            End If
            'Application.DoEvents()
            m_bSkipEvents = True
            If Me.FormBorderStyle = FormBorderStyle.None Then
                PictureBox1.Width = Me.Width
                PictureBox1.Height = Me.Height
            Else
                PictureBox1.Width = Me.ClientSize.Width
                PictureBox1.Height = Me.ClientSize.Height
            End If
            Application.DoEvents()
            m_bSkipEvents = False

            Debug.WriteLine("redrawing after space")
            If Not m_graphics Is Nothing Then
                m_graphics.Clear(System.Drawing.Color.Black)
                m_graphics = PictureBox1.CreateGraphics
                LoadAndCalculateImage(True)
                DisplayImage()
            End If
            m_bSkipEvents = False
            Debug.WriteLine("redrawn after space")
        End If
    End Sub

    Public Sub SlideShow(ByVal alPics As String())

        m_alPics = alPics
        m_lMax = alPics.Length
        m_strPreviousPath = Path.GetDirectoryName(m_alPics(m_lIndex))
        m_lIndexF = 1
        m_lMaxF = htPathCounts(m_strPreviousPath)
        m_lPathCountIndex = 1
        LoadAndCalculateImage()

        m_graphics = PictureBox1.CreateGraphics
        m_BlackBrush = New System.Drawing.SolidBrush(System.Drawing.Color.Black)

        Timer1.Interval = 100
        Timer1.Enabled = True

    End Sub

    Private Sub LoadAndCalculateImage(Optional ByVal bResize As Boolean = False)
        m_bLoading = True

        Static lImageWidth As Long
        Static lImageHeight As Long
        If Not bResize Then
            Dim bError As Boolean = True
            While bError
                bError = False
                Try
                    If Not MyImage Is Nothing Then
                        MyImage.Dispose()
                    End If
                    
                    MyImage = New System.Drawing.Bitmap(m_alPics(m_lIndex))
                Catch ex As Exception
                    bError = True
                    If m_lIndex < m_lMax Then
                        m_lIndex += 1
                        m_lIndexF += 1
                        If m_strPreviousPath <> Path.GetDirectoryName(m_alPics(m_lIndex)) Then
                            m_strPreviousPath = Path.GetDirectoryName(m_alPics(m_lIndex))
                            m_lMaxF = htPathCounts(m_strPreviousPath)
                            m_lPathCountIndex += 1
                            m_lIndexF = 1
                        End If
                    End If
                End Try
            End While

            lImageWidth = MyImage.Width
            lImageHeight = MyImage.Height
        Else
            m_bMustRecalculate = True
            lImageWidth = MyImageVisible.Width
            lImageHeight = MyImageVisible.Height
        End If

        Dim dblImageRatio As Double
        dblImageRatio = lImageWidth / lImageHeight
        Dim dblWindowRatio As Double
        Dim lPBWidth As Long
        Dim lPBHeight As Long
        lPBWidth = PictureBox1.Width
        lPBHeight = PictureBox1.Height
        dblWindowRatio = lPBWidth / lPBHeight
        Dim lX As Long = 0
        Dim lY As Long = 0
        Dim lWidth As Long = lPBWidth
        Dim lHeight As Long = lPBHeight
        If lImageWidth > lPBWidth OrElse lImageHeight > lPBHeight Then
            'shrink
            If dblImageRatio > dblWindowRatio Then
                'image is wider than window
                lHeight = lImageHeight * (lWidth / lImageWidth)
                lY = (lPBHeight - lHeight) / 2
            Else
                'image is higher than window
                lWidth = lImageWidth * (lHeight / lImageHeight)
                lX = (lPBWidth - lWidth) / 2
            End If
        Else
            'normal
            lWidth = lImageWidth
            lHeight = lImageHeight
            lX = (lPBWidth - lWidth) / 2
            lY = (lPBHeight - lHeight) / 2
        End If
        m_lX = lX
        m_lY = lY
        m_lWidth = lWidth
        m_lHeight = lHeight

        m_bLoading = False
    End Sub

    Private Sub DisplayImage()
        'draw image
        m_graphics.DrawImage(MyImageVisible, m_lX, m_lY, m_lWidth, m_lHeight)
    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick
        If Not m_bStarted Then
            m_bStarted = True
            Timer1.Interval = lDuration * 1000
        End If
        'pause timer and wait for next image to load
        Timer1.Enabled = False
        While m_bLoading
            Application.DoEvents()
        End While

        Dim MyRegion As Region
        MyRegion = New Region(New Rectangle(m_lX2, m_lY2, m_lWidth2, m_lHeight2))
        If Not MyImageVisible Is Nothing Then MyImageVisible.Dispose()
        MyImageVisible = MyImage.Clone
        If m_bMustRecalculate Then
            LoadAndCalculateImage(True)
            m_bMustRecalculate = False
        End If
        MyRegion.Exclude(New Rectangle(m_lX, m_lY, m_lWidth, m_lHeight))
        Dim OldRegion As Region = m_graphics.Clip
        m_graphics.Clip = MyRegion
        m_graphics.Clear(Color.Black)
        m_graphics.Clip = OldRegion
        MyRegion.Dispose()
        DisplayImage()
        m_lX2 = m_lX
        m_lY2 = m_lY
        m_lWidth2 = m_lWidth
        m_lHeight2 = m_lHeight
        'refresh title bar
        SetCaption(m_alPics(m_lIndex))
        'load next image
        If m_lIndex < m_lMax - 1 Then
            'start timer again
            Timer1.Enabled = True
            m_lIndex += 1
            m_lIndexF += 1
            If m_strPreviousPath <> Path.GetDirectoryName(m_alPics(m_lIndex)) Then
                m_strPreviousPath = Path.GetDirectoryName(m_alPics(m_lIndex))
                m_lMaxF = htPathCounts(m_strPreviousPath)
                m_lPathCountIndex += 1
                m_lIndexF = 1
            End If
            LoadAndCalculateImage()
        Else
            Timer1.Enabled = False
        End If
    End Sub

    Private Sub PictureBox1_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles PictureBox1.Paint
        If m_bSkipEvents Then Exit Sub
        If Not MyImageVisible Is Nothing Then
            Application.DoEvents()
            Debug.WriteLine("picturebox paint")
            DisplayImage()
        End If
    End Sub

    Private Sub Form1_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
        If Not MyImageVisible Is Nothing AndAlso Not m_graphics Is Nothing Then
            Application.DoEvents()
            DisplayImage()
        End If
    End Sub

    Private Sub Form1_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Resize
        m_lWindowWidth = Me.Width
        m_lWindowHeight = Me.Height
        If m_bSkipEvents Then Exit Sub
        Debug.WriteLine("form resize")
        m_bSkipEvents = True
        PictureBox1.Width = Me.ClientSize.Width
        PictureBox1.Height = Me.ClientSize.Height
        Application.DoEvents()
        m_bSkipEvents = False
        If Me.WindowState = FormWindowState.Minimized Then
            Timer1.Enabled = False
        Else
            If m_bStarted Then
                Timer1.Enabled = True
                If Not m_graphics Is Nothing Then
                    Dim MyRegion As Region
                    MyRegion = New Region(New Rectangle(m_lX, m_lY, m_lWidth, m_lHeight))
                    m_graphics = PictureBox1.CreateGraphics
                    LoadAndCalculateImage(True)
                    MyRegion.Exclude(New Rectangle(m_lX, m_lY, m_lWidth, m_lHeight))
                    Dim OldRegion As Region = m_graphics.Clip
                    m_graphics.Clip = MyRegion
                    m_graphics.Clear(Color.Black)
                    m_graphics.Clip = OldRegion
                    DisplayImage()
                Else
                    m_graphics = PictureBox1.CreateGraphics
                End If
            End If
        End If
    End Sub

End Class
