{
  "targets": [
    {
      "target_name": "JSCalcEng",
      "sources": [ "CalcConsoleApp.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }		
  ]
}
