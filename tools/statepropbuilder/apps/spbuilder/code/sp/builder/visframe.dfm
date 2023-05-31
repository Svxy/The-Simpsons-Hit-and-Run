object Frame2: TFrame2
  Left = 0
  Top = 0
  Width = 152
  Height = 43
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 152
    Height = 43
    Align = alClient
    BevelInner = bvLowered
    TabOrder = 0
    object NameLabel: TLabel
      Left = 8
      Top = 5
      Width = 26
      Height = 13
      Caption = 'name'
      Color = clSilver
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object VisibilityCheckBox: TCheckBox
      Left = 8
      Top = 21
      Width = 97
      Height = 17
      Caption = 'Visible'
      ParentShowHint = False
      ShowHint = False
      TabOrder = 0
      OnClick = VisibilityCheckBoxClick
    end
  end
end
