Network의 CRC Decoder와 Encoder를 구현하였습니다.
1. datastream을 encoder로 바꾼다.
2. encoder를 통과한 데이터를 linksim으로 통과시킨다.
3. linksim으로 통과시킨 데이터를 decoder로 통과시킨다.
4. linksim을 통과한 후 데이터가 정상적이었다면 변화없는 datastream이 나온다.
