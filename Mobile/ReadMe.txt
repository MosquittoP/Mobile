Moblie

기반 언어 : C

Raspberry pi 3를 이용하여 작동되는 모빌입니다.
개발 환경은 Linux(mount 환경), 언어는 C이며 Wiring Pi를 사용하지 않고 커널 디바이스 드라이버를 개발했습니다.
현재 기능은 스위치를 사용한 On, Off 시 LED점등 및 모터가 작동하며, 동작시 8*8 Dot Matrix 작동, 내장 스피커를 이용한 mp3파일 재생이 있습니다.
디바이스 드라이버는 key(스위치~LED) driver, moter driver, dot driver가 있습니다.
리눅스 환경에서 작동 시 test_key2를 실행한 후 정상 작동합니다.