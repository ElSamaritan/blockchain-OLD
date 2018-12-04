# Author: Sankarsan Kampa (a.k.a. k3rn31p4nic)
# License: MIT

$STATUS=$args[0]
$WEBHOOK_URL=$args[1]

if (!$WEBHOOK_URL) {
  Write-Output "WARNING!!"
  Write-Output "You need to pass the WEBHOOK_URL environment variable as the second argument to this script."
  Write-Output "For details & guide, visit: https://github.com/DiscordHooks/appveyor-discord-webhook"
  Exit
}

Write-Output "[Webhook]: Sending webhook to Discord..."

Switch ($STATUS) {
  "success" {
    $EMBED_COLOR=3066993
    $STATUS_MESSAGE="Passed"
    $URL=$env:CI_PIPELINE_URL
    Break
  }
  "failure" {
    $EMBED_COLOR=15158332
    $STATUS_MESSAGE="Failed"
    $URL=$env:CI_JOB_URL
    Break
  }
  default {
    Write-Output "Default!"
    Break
  }
}
$AVATAR="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/GitLab_Logo.svg/1108px-GitLab_Logo.svg.png"

if (!$env:CI_COMMIT_SHA) {
  $env:CI_COMMIT_SHA="$(git log -1 --pretty="%H")"
}

$AUTHOR_NAME="$(git log -1 "$env:CI_COMMIT_SHA" --pretty="%aN")"
$COMMITTER_NAME="$(git log -1 "$env:CI_COMMIT_SHA" --pretty="%cN")"
$COMMIT_SUBJECT="$(git log -1 "$env:CI_COMMIT_SHA" --pretty="%s")"
$COMMIT_MESSAGE="$(git log -1 "$env:CI_COMMIT_SHA" --pretty="%b")"

if ($AUTHOR_NAME -eq $COMMITTER_NAME) {
  $CREDITS="$AUTHOR_NAME authored & committed"
}
else {
  $CREDITS="$AUTHOR_NAME authored & $COMMITTER_NAME committed"
}

$BUILD_VERSION = [uri]::EscapeDataString($env:APPVEYOR_BUILD_VERSION)
$TIMESTAMP="$(Get-Date -format s)Z"
$WEBHOOK_DATA="{
  ""username"": """",
  ""avatar_url"": ""$AVATAR"",
  ""embeds"": [ {
    ""color"": $EMBED_COLOR,
    ""author"": {
      ""name"": ""Job #$env:CI_JOB_ID ($env:CI_JOB_NAME) $STATUS_MESSAGE - $env:CI_PROJECT_PATH_SLUG"",
      ""url"": ""$env:CI_JOB_URL"",
      ""icon_url"": ""$AVATAR""
    },
    ""title"": ""$COMMIT_SUBJECT"",
    ""url"": ""$URL"",
    ""description"": ""$COMMIT_MESSAGE $CREDITS"",
    ""fields"": [
      {
        ""name"": ""Commit"",
        ""value"": ""[``$($env:CI_COMMIT_SHA.substring(0, 7))``](https://gitlab.com/$($env:CI_PROJECT_PATH_SLUG)/commit/$($env:CI_COMMIT_SHA))"",
        ""inline"": true
      },
      {
        ""name"": ""Branch/Tag"",
        ""value"": ""[``$env:APPVEYOR_REPO_BRANCH``](https://gitlab.com/$($env:CI_PROJECT_PATH_SLUG)/tree/$($env:I_COMMIT_REF_SLUG))"",
        ""inline"": true
      }
    ],
    ""timestamp"": ""$TIMESTAMP""
  } ]
}"

Invoke-RestMethod -Uri "$WEBHOOK_URL" -Method "POST" -UserAgent "AppVeyor-Webhook" `
  -ContentType "application/json" -Header @{"X-Author"="k3rn31p4nic#8383"} `
  -Body $WEBHOOK_DATA

Write-Output "[Webhook]: Successfully sent the webhook."