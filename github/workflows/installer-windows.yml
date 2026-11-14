
name: Build La Pauta Analyzer Pro Installer

on:
  workflow_dispatch:
  push:
    branches: [ main, master ]

jobs:
  build-installer:
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Install Inno Setup
        shell: pwsh
        run: choco install innosetup --yes --no-progress

      - name: Configure
        run: cmake -S . -B build -A x64

      - name: Build VST3
        run: cmake --build build --config Release --parallel

      - name: Stage VST3 bundle
        run: cmake --install build --config Release --prefix installer/staging --component VST3

      - name: Verify staged plugin
        shell: pwsh
        run: |
          $plugin = "installer/staging/La Pauta Analyzer Pro.vst3"

          if (!(Test-Path $plugin)) {
            Write-Error "No se encontró el plugin VST3: $plugin"
            Get-ChildItem -Recurse build | Select-Object -ExpandProperty FullName
            exit 1
          }

      - name: Create Windows installer
        shell: pwsh
        run: |
          New-Item -ItemType Directory -Force -Path output | Out-Null

          & "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "installer/LaPautaAnalyzerPro.iss"

          if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
          }

      - name: Upload installer
        uses: actions/upload-artifact@v4
        with:
          name: La-Pauta-Analyzer-Pro-Windows-Installer
          path: output/LaPautaAnalyzerPro_Setup.exe
          if-no-files-found: error
          retention-days: 30
          
