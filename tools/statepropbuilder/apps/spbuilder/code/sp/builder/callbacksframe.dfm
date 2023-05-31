object Frame4: TFrame4
  Left = 0
  Top = 0
  Width = 286
  Height = 71
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 286
    Height = 71
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object Label2: TLabel
      Left = 8
      Top = 10
      Width = 41
      Height = 13
      Caption = 'Callback'
    end
    object Label1: TLabel
      Left = 8
      Top = 35
      Width = 43
      Height = 13
      Caption = 'On frame'
    end
    object CallbackComboBox: TComboBox
      Left = 56
      Top = 5
      Width = 121
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = CallbackComboBoxChange
    end
    object OnFrameEditBox: TEdit
      Left = 56
      Top = 30
      Width = 49
      Height = 21
      TabOrder = 1
      OnChange = OnFrameEditBoxChange
      OnExit = OnFrameEditBoxExit
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
