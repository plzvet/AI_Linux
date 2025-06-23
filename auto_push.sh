#!/bin/bash

# === 기본 설정 ===
BRANCH=$(git rev-parse --abbrev-ref HEAD)
echo "현재 브랜치: $BRANCH"

# === 추적 대상 확장자 목록 ===
EXTENSIONS="*.c *.h *.cpp *.py *.sh *.java *.md"

# === 파일 추가 ===
echo "▶️ 코드 파일만 추가 중..."
git add --intent-to-add $EXTENSIONS 2>/dev/null
git status

# === 커밋 메시지 입력 ===
read -p "📌 커밋 메시지를 입력하세요: " msg

# === 커밋 ===
git commit -am "$msg"

# === 푸시 ===
echo "🚀 GitHub에 푸시 중..."
git push origin "$BRANCH"

