object Frame3: TFrame3
  Left = 0
  Top = 0
  Width = 212
  Height = 62
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 212
    Height = 62
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 10
      Width = 44
      Height = 13
      Caption = 'On event'
    end
    object Label2: TLabel
      Left = 8
      Top = 35
      Width = 50
      Height = 13
      Caption = 'go to state'
    end
    object EventComboBox: TComboBox
      Left = 64
      Top = 5
      Width = 113
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = EventComboBoxChange
    end
    object ToFrameEditBox: TEdit
      Left = 64
      Top = 30
      Width = 41
      Height = 21
      TabOrder = 1
      OnChange = ToFrameEditBoxChange
      OnExit = ToFrameEditBoxExit
    end
    object Button1: TButton
      Left = 110
      Top = 30
      Width = 67
      Height = 23
      Caption = 'delete'
      TabOrder = 2
      OnClick = Button1Click
    end
  end
end
