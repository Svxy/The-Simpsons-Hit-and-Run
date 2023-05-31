object MainForm: TMainForm
  Left = 197
  Top = 118
  Width = 1262
  Height = 869
  Caption = 'State Animation Builder'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 0
    Top = 628
    Width = 1254
    Height = 3
    Cursor = crVSplit
    Align = alBottom
  end
  object Splitter1: TSplitter
    Left = 230
    Top = 0
    Width = 3
    Height = 628
    Cursor = crHSplit
  end
  object Panel2: TPanel
    Left = 0
    Top = 631
    Width = 1254
    Height = 192
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 1254
      Height = 33
      Align = alTop
      BevelInner = bvLowered
      TabOrder = 0
      object Label1: TLabel
        Left = 2
        Top = 2
        Width = 31
        Height = 29
        Align = alLeft
        Caption = ' Prop: '
      end
      object PropNameLabel: TLabel
        Left = 33
        Top = 2
        Width = 24
        Height = 29
        Align = alLeft
        Caption = 'none'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -12
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label2: TLabel
        Left = 124
        Top = 2
        Width = 26
        Height = 29
        Align = alLeft
        Caption = '   Of: '
      end
      object NumPropStatesLabel: TLabel
        Left = 150
        Top = 2
        Width = 6
        Height = 29
        Align = alLeft
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -12
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label3: TLabel
        Left = 57
        Top = 2
        Width = 61
        Height = 29
        Align = alLeft
        Caption = '          State: '
      end
      object CurrentStateLabel: TLabel
        Left = 118
        Top = 2
        Width = 6
        Height = 29
        Align = alLeft
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -12
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label4: TLabel
        Left = 720
        Top = 7
        Width = 29
        Height = 13
        Caption = 'Frame'
      end
      object PlayButton: TButton
        Left = 560
        Top = 5
        Width = 30
        Height = 20
        Hint = 'Play / Pause'
        Caption = '| |'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = PlayButtonClick
      end
      object ForwardStepButton: TButton
        Left = 592
        Top = 5
        Width = 30
        Height = 20
        Hint = 'Step Forward One Frame'
        Caption = '> |'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = ForwardStepButtonClick
      end
      object BackStepButton: TButton
        Left = 528
        Top = 5
        Width = 30
        Height = 20
        Hint = 'Step Back One Frame'
        Caption = '| <'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = BackStepButtonClick
      end
      object CurrentFrameWindow: TEdit
        Left = 760
        Top = 4
        Width = 60
        Height = 21
        Hint = 'Current Frame, seconds'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
      end
      object PrevStatebutton: TButton
        Left = 495
        Top = 5
        Width = 30
        Height = 20
        Hint = 'Previous State'
        Caption = '| <<'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = PrevStatebuttonClick
      end
      object NextStateButton: TButton
        Left = 624
        Top = 5
        Width = 30
        Height = 20
        Hint = 'Next State'
        Caption = '>> |'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
        OnClick = NextStateButtonClick
      end
    end
    object Panel4: TScrollBox
      Left = 0
      Top = 33
      Width = 1254
      Height = 159
      Align = alClient
      BorderStyle = bsNone
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 230
    Height = 628
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 1
    object PageControl1: TPageControl
      Left = 0
      Top = 0
      Width = 230
      Height = 628
      ActivePage = TabSheet1
      Align = alClient
      MultiLine = True
      TabOrder = 0
      object TabSheet1: TTabSheet
        Caption = 'Drawables'
        object Panel12: TPanel
          Left = 0
          Top = 0
          Width = 222
          Height = 41
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object Button6: TButton
            Left = 8
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Hide All'
            TabOrder = 0
            OnClick = Button6Click
          end
          object Button7: TButton
            Left = 96
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Show All'
            TabOrder = 1
            OnClick = Button7Click
          end
        end
        object Panel5: TScrollBox
          Left = 0
          Top = 41
          Width = 222
          Height = 559
          Align = alClient
          BorderStyle = bsNone
          TabOrder = 1
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Events'
        ImageIndex = 1
        object EventsPanel: TPanel
          Left = 0
          Top = 0
          Width = 222
          Height = 41
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object Button1: TButton
            Left = 8
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Add Event'
            TabOrder = 0
            OnClick = Button1Click
          end
        end
        object Panel6: TScrollBox
          Left = 0
          Top = 41
          Width = 222
          Height = 559
          Align = alClient
          BorderStyle = bsNone
          TabOrder = 1
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'Callbacks'
        ImageIndex = 2
        object Panel7: TPanel
          Left = 0
          Top = 0
          Width = 222
          Height = 600
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object Panel8: TPanel
            Left = 0
            Top = 0
            Width = 222
            Height = 41
            Align = alTop
            BevelOuter = bvNone
            TabOrder = 0
            object Button2: TButton
              Left = 8
              Top = 8
              Width = 75
              Height = 25
              Caption = 'Add Callback'
              TabOrder = 0
              OnClick = Button2Click
            end
          end
          object CallbackPanel: TScrollBox
            Left = 0
            Top = 41
            Width = 222
            Height = 559
            Align = alClient
            BorderStyle = bsNone
            TabOrder = 1
          end
        end
      end
      object TabSheet4: TTabSheet
        Caption = 'State'
        ImageIndex = 3
        object Panel11: TPanel
          Left = 0
          Top = 0
          Width = 222
          Height = 265
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object Label10: TLabel
            Left = 7
            Top = 10
            Width = 34
            Height = 16
            Caption = 'State:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
          end
          object StateStateLabel: TLabel
            Left = 42
            Top = 10
            Width = 7
            Height = 16
            Caption = '0'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clNavy
            Font.Height = -15
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
          end
          object Label6: TLabel
            Left = 8
            Top = 130
            Width = 44
            Height = 13
            Caption = 'To State:'
          end
          object Label8: TLabel
            Left = 8
            Top = 154
            Width = 49
            Height = 13
            Caption = 'On Frame:'
          end
          object AutoTransitionCheckBox: TCheckBox
            Left = 8
            Top = 105
            Width = 97
            Height = 17
            Caption = 'Auto Transition'
            TabOrder = 0
            OnClick = AutoTransitionCheckBoxClick
          end
          object ToStateEditBox: TEdit
            Left = 64
            Top = 126
            Width = 41
            Height = 21
            TabOrder = 1
            OnChange = ToStateEditBoxChange
            OnExit = ToStateEditBoxExit
          end
          object OnFrameEditBox: TEdit
            Left = 64
            Top = 150
            Width = 41
            Height = 21
            TabOrder = 2
            OnChange = OnFrameEditBoxChange
            OnExit = OnFrameEditBoxExit
          end
          object Button4: TButton
            Left = 8
            Top = 64
            Width = 75
            Height = 25
            Caption = 'Delete State'
            TabOrder = 3
            OnClick = Button4Click
          end
          object Button3: TButton
            Left = 8
            Top = 32
            Width = 75
            Height = 25
            Caption = 'Add State'
            TabOrder = 4
            OnClick = Button3Click
          end
          object PDDICheckBox: TCheckBox
            Left = 8
            Top = 184
            Width = 97
            Height = 17
            Caption = 'PDDI Stats'
            TabOrder = 5
            OnClick = PDDICheckBoxClick
          end
        end
      end
    end
  end
  object Panel9: TPanel
    Left = 233
    Top = 0
    Width = 1021
    Height = 628
    Align = alClient
    BevelOuter = bvNone
    Caption = 'Panel9'
    TabOrder = 2
    object P3DClientPanel: TPanel
      Left = 0
      Top = 33
      Width = 1021
      Height = 595
      Align = alClient
      BevelInner = bvLowered
      Caption = 'P3DClientPanel'
      Color = clCaptionText
      TabOrder = 0
      OnResize = P3DClientPanelResize
      object P3DPanel: TPanel
        Left = 2
        Top = 2
        Width = 1065
        Height = 793
        BevelOuter = bvNone
        Color = clActiveCaption
        TabOrder = 0
        OnMouseDown = P3DPanelMouseDown
        OnMouseMove = P3DPanelMouseMove
        OnMouseUp = P3DPanelMouseUp
      end
    end
    object Panel10: TPanel
      Left = 0
      Top = 0
      Width = 1021
      Height = 33
      Align = alTop
      BevelInner = bvLowered
      TabOrder = 1
      object Label5: TLabel
        Left = 10
        Top = 7
        Width = 41
        Height = 20
        Caption = 'Prop: '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object PropNameLabelTop: TLabel
        Left = 56
        Top = 7
        Width = 36
        Height = 20
        Caption = 'none'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object NumPropStatesLabelTop: TLabel
        Left = 309
        Top = 7
        Width = 9
        Height = 20
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label9: TLabel
        Left = 210
        Top = 7
        Width = 43
        Height = 20
        Caption = 'State:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object CurrentStateLabelTop: TLabel
        Left = 256
        Top = 7
        Width = 9
        Height = 20
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 286
        Top = 7
        Width = 21
        Height = 20
        Caption = 'Of:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -16
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object ApplicationEvents1: TApplicationEvents
    OnIdle = ApplicationEvents1Idle
    Left = 1040
    Top = 72
  end
  object MainMenu1: TMainMenu
    Images = ImageList1
    Left = 1072
    Top = 72
    object N1: TMenuItem
      Caption = '&File'
      object Open1: TMenuItem
        Action = FileOpen
      end
      object Save1: TMenuItem
        Action = FileSave
      end
      object Exit1: TMenuItem
        Action = FileExit
      end
    end
    object Edit1: TMenuItem
      Caption = '&Edit'
      object B1: TMenuItem
        Caption = '&Background Color'
        OnClick = B1Click
      end
      object P3DBackgroundArt1: TMenuItem
        Caption = '&P3D Background Art'
        OnClick = P3DBackgroundArt1Click
      end
    end
    object Help1: TMenuItem
      Caption = '&Help'
      object About1: TMenuItem
        Action = HelpAbout
      end
    end
  end
  object ActionList1: TActionList
    Left = 1136
    Top = 72
    object FileOpen: TAction
      Category = 'File'
      Caption = '&Open'
      Hint = 'Open File'
      ImageIndex = 0
      OnExecute = FileOpenExecute
    end
    object FileSave: TAction
      Category = 'File'
      Caption = '&Save'
      Hint = 'Save File'
      ImageIndex = 1
      OnExecute = FileSaveExecute
    end
    object FileExit: TAction
      Category = 'File'
      Caption = '&Exit'
      Hint = 'Exit Application'
    end
    object HelpAbout: TAction
      Category = 'Help'
      Caption = '&About'
    end
  end
  object ImageList1: TImageList
    Left = 1104
    Top = 72
    Bitmap = {
      494C010102000400040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000084
      8400000000000000000000000000000000000000000000000000C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000084
      8400008484000084840000848400008484000084840000848400008484000000
      0000000000000000000000000000000000000000000000000000008484000084
      8400000000000000000000000000000000000000000000000000C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00000000000084
      8400008484000084840000848400008484000084840000848400008484000084
      8400000000000000000000000000000000000000000000000000008484000084
      8400000000000000000000000000000000000000000000000000C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF000000
      0000008484000084840000848400008484000084840000848400008484000084
      8400008484000000000000000000000000000000000000000000008484000084
      8400000000000000000000000000000000000000000000000000000000000000
      0000000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00000000000084840000848400008484000084840000848400008484000084
      8400008484000084840000000000000000000000000000000000008484000084
      8400008484000084840000848400008484000084840000848400008484000084
      8400008484000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000084
      8400000000000000000000000000000000000000000000000000000000000000
      0000008484000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C600000000000084840000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008484000000
      0000C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6C600C6C6
      C60000000000C6C6C60000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF00FFFFFFFF00000000FFFFC00100000000
      001F800100000000000F80010000000000078001000000000003800100000000
      00018001000000000000800100000000001F800100000000001F800100000000
      001F8001000000008FF1800100000000FFF9800100000000FF75800100000000
      FF8F800100000000FFFFFFFF0000000000000000000000000000000000000000
      000000000000}
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'p3d'
    Filter = 'P3D|*.p3d|All files|*.*'
    Title = 'Open File'
    Left = 1042
    Top = 112
  end
  object ColorDialog1: TColorDialog
    Ctl3D = True
    Left = 1075
    Top = 115
  end
end
