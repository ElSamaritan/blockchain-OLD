param(
    [Parameter(Mandatory=$true)][string]$Container,
    [Parameter(Mandatory=$true)][string]$Name,
    $JumpIn = $false
)

$RootDir = $(git rev-parse --show-toplevel)
$BuildDir = "$RootDir\.build\$Name"

Write-Host "[--XI--] Container: $Container"
Write-Host "[--XI--] Root Dir : $RootDir"
Write-Host "[--XI--] Build Dir: $BuildDir"

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

Push-Location $RootDir

docker run --rm -it -v "${PWD}:/source" -v "${PWD}/.build/$($Name):/build" -w "/build" $Container bash /source/scripts/dev/Build.sh
if($JumpIn)
{
    docker run --rm -it -v "${PWD}:/source" -v "${PWD}/.build/$($Name):/build" -w "/build" $Container bash
}

Pop-Location