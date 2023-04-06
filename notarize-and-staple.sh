#!/bin/bash
APPLICATION_BUNDLE_PATH=$1
ZIPPED_BUNDLE_PATH=$2
ditto -c -k --keepParent "${APPLICATION_BUNDLE_PATH}" "${ZIPPED_BUNDLE_PATH}"
xcrun notarytool submit "${ZIPPED_BUNDLE_PATH}" --apple-id "${APP_CONNECT_NOTARIZATION_USERNAME}" --password "${APP_CONNECT_NOTARIZATION_PASSWORD}" --team-id "${TEAM_IDENTITY}" --wait
xcrun stapler staple "${APPLICATION_BUNDLE_PATH}"
ditto -c -k --keepParent "${APPLICATION_BUNDLE_PATH}" "${ZIPPED_BUNDLE_PATH}"