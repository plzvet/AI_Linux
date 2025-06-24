#!/bin/bash

# === 현재 브랜치 확인 ===
BRANCH=$(git rev-parse --abbrev-ref HEAD)
echo "현재 브랜치: $BRANCH"

# === 전부 추가 ===
echo "📂 모든 파일을 Git에 추가합니다..."
git add -A

# === 상태 확인 ===
git status

# === 커밋 메시지 입력 ===
read -p "📝 커밋 메시지를 입력하세요: " msg

# === 커밋 ===
git commit -m "$msg"

# === 푸시 ===
echo "🚀 GitHub에 푸시 중..."
git push origin "$BRANCH"
