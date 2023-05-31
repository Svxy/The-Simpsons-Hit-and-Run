object Frame1: TFrame1
  Left = 0
  Top = 0
  Width = 1066
  Height = 30
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1066
    Height = 30
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object FrameControllerNameLabel: TLabel
      Left = 10
      Top = 7
      Width = 24
      Height = 13
      Caption = 'none'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -8
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label1: TLabel
      Left = 130
      Top = 7
      Width = 52
      Height = 13
      Caption = 'Frame: Min'
    end
    object Label3: TLabel
      Left = 256
      Top = 7
      Width = 20
      Height = 13
      Caption = 'Max'
    end
    object Label4: TLabel
      Left = 720
      Top = 7
      Width = 29
      Height = 13
      Caption = 'Frame'
    end
    object Label5: TLabel
      Left = 352
      Top = 8
      Width = 31
      Height = 13
      Caption = 'Speed'
    end
    object MinFrameEditBox: TEdit
      Left = 186
      Top = 4
      Width = 60
      Height = 21
      TabOrder = 0
      OnChange = MinFrameEditBoxChange
      OnExit = EditBoxExit
    end
    object MaxFrameEditBox: TEdit
      Left = 280
      Top = 4
      Width = 60
      Height = 21
      TabOrder = 1
      OnChange = MaxFrameEditBoxChange
      OnExit = EditBoxExit
    end
    object CyclicCheckBox: TCheckBox
      Left = 456
      Top = 7
      Width = 49
      Height = 17
      Caption = 'Cycle'
      TabOrder = 2
      OnClick = CyclicCheckBoxClick
    end
    object CurrentFrameLabel: TEdit
      Left = 760
      Top = 4
      Width = 60
      Height = 21
      TabOrder = 3
    end
    object holdFrameCheckbox: TCheckBox
      Left = 560
      Top = 7
      Width = 57
      Height = 17
      Caption = 'Hold'
      TabOrder = 4
      OnClick = holdFrameCheckboxClick
    end
    object cycleForEditBox: TEdit
      Left = 503
      Top = 4
      Width = 39
      Height = 21
      Enabled = False
      TabOrder = 5
      Text = '0'
      OnChange = cycleForEditBoxChange
      OnExit = EditBoxExit
    end
    object speedEditBox: TEdit
      Left = 387
      Top = 4
      Width = 57
      Height = 21
      TabOrder = 6
      OnChange = speedEditBoxChange
      OnExit = EditBoxExit
    end
  end
end
