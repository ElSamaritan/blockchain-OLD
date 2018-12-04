#!/bin/bash

if [ -z "$2" ]; then
  echo -e "WARNING!!\nYou need to pass the WEBHOOK_URL environment variable as the second argument to this script.\nFor details & guide, visit: https://github.com/DiscordHooks/travis-ci-discord-webhook" && exit
fi

echo -e "[Webhook]: Sending webhook to Discord...\\n";

case $1 in
  "success" )
    EMBED_COLOR=3066993
    STATUS_MESSAGE="Passed"
    AVATAR="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/GitLab_Logo.svg/1108px-GitLab_Logo.svg.png"
    URL="$CI_PIPELINE_URL"
    ;;

  "failure" )
    EMBED_COLOR=15158332
    STATUS_MESSAGE="Failed"
    AVATAR="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/GitLab_Logo.svg/1108px-GitLab_Logo.svg.png"
    URL="$CI_JOB_URL"
    ;;

  * )
    EMBED_COLOR=0
    STATUS_MESSAGE="Status Unknown"
    AVATAR="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/GitLab_Logo.svg/1108px-GitLab_Logo.svg.png"
    URL=""
    ;;
esac

AUTHOR_NAME="$(git log -1 "$CI_COMMIT_SHA" --pretty="%aN")"
COMMITTER_NAME="$(git log -1 "$CI_COMMIT_SHA" --pretty="%cN")"
COMMIT_SUBJECT="$(git log -1 "$CI_COMMIT_SHA" --pretty="%s")"
COMMIT_MESSAGE="$(git log -1 "$CI_COMMIT_SHA" --pretty="%b")"

if [ "$AUTHOR_NAME" == "$COMMITTER_NAME" ]; then
  CREDITS="$AUTHOR_NAME authored & committed"
else
  CREDITS="$AUTHOR_NAME authored & $COMMITTER_NAME committed"
fi

TIMESTAMP=$(date --utc +%FT%TZ)
WEBHOOK_DATA='{
  "username": "",
  "avatar_url": "https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/GitLab_Logo.svg/1108px-GitLab_Logo.svg.png",
  "embeds": [ {
    "color": '$EMBED_COLOR',
    "author": {
      "name": "Job #'"$CI_JOB_ID"' ('"$CI_JOB_NAME"') '"$STATUS_MESSAGE"' - '"$CI_PROJECT_PATH_SLUG"'",
      "url": "'"$CI_JOB_URL"'",
      "icon_url": "'$AVATAR'"
    },
    "title": "'"$COMMIT_SUBJECT"'",
    "url": "'"$URL"'",
    "description": "'"${COMMIT_MESSAGE//$'\n'/ }"\\n\\n"$CREDITS"'",
    "fields": [
      {
        "name": "Commit",
        "value": "'"[\`${CI_COMMIT_SHA:0:7}\`](https://gitlab.com/$CI_PROJECT_PATH_SLUG/commit/$CI_COMMIT_SHA)"'",
        "inline": true
      },
      {
        "name": "Branch/Tag",
        "value": "'"[\`$CI_COMMIT_REF_NAME\`](https://gitlab.com/$CI_PROJECT_PATH_SLUG/tree/$CI_COMMIT_REF_SLUG)"'",
        "inline": true
      }
    ],
    "timestamp": "'"$TIMESTAMP"'"
  } ]
}'

(curl --fail --progress-bar -A "GitLabCI-Webhook" -H Content-Type:application/json -H X-Author:k3rn31p4nic#8383 -d "$WEBHOOK_DATA" "$2" \
  && echo -e "\\n[Webhook]: Successfully sent the webhook.") || echo -e "\\n[Webhook]: Unable to send webhook."