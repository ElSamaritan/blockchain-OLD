# From http://devhawk.net/blog/2010/1/22/fixing-powershells-busted-resolve-path-cmdlet

function Get-Resolve-Path {
    param (
        [string] $FileName
    )

    $FileName = Resolve-Path $FileName -ErrorAction SilentlyContinue `
                                       -ErrorVariable frperror
    if (-Not($FileName)) {
        $FileName = $frperror[0].TargetObject
    }

    return $FileName
}