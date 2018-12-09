$HeaderFiles = Get-ChildItem -Path .\src -Recurse -Filter *.h
$CPPSourceFiles = Get-ChildItem -Path .\src -Recurse -Filter *.cpp
$CSourceFiles = Get-ChildItem -Path .\src -Recurse -Filter *.c

$Files = $HeaderFiles + $CPPSourceFiles + $CSourceFiles

$Encoding = New-Object System.Text.UTF8Encoding $True
foreach ($file in $Files) {
    Write-Host "Processing '$($file.FullName)'"
    clang-format -i --style=file $($file.FullName)
    $content = [IO.File]::ReadAllText($file.FullName);
    Write-Host "Speichere Datei in UTF-8(BOM)..."
    [IO.File]::WriteAllText($file.Fullname, $content, $Encoding)
}
