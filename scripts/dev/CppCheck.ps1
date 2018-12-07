$RootDir = $(git rev-parse --show-toplevel)
$Container = "registry.gitlab.com/galaxia-project/blockchain/develop-environments/cppcheck"

Write-Host "[--XI--] Container: $Container"
Write-Host "[--XI--] Root Dir : $RootDir"

Push-Location $RootDir

docker run --rm -it -v "${PWD}:/source" -w "/source" -e CI_PROJECT_DIR=/source $Container bash ./scripts/ci/CppCheck.sh

Pop-Location