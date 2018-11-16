🔌 다 기능 전원 제어 장치 Git Hub
==========================================
## Multi-Function Power Supply Device 

주제 : DE1_SOC 보드를 이용한 원격 전원제어장치

목적 : 하드웨어를 직접 제작해서 원격 전원제어를 할 수 있게 한다.

정리 : 본체에 콘센트, 빵판, de1-SOC보드가 들어가게 해서
콘센트(3구)의 선중에서 1개씩 릴레이를 연결해서 빵판에다가 꽂는다.
DE1-S0C상에서 GPIO의 VCC로 신호를 줄지 안줄지를 결정한다.
이때 접지를 해줘야 하므로 GND쪽으로도 연결해준다.
본체에서 USB를 꽂는 포트를 만들어서 컨트롤러를 꽂으면 컨트롤러가 되고
컨트롤러를 꽂지 않으면 KEY 인터럽트를 사용해서 직접제어를 할 수 있다.
또한 모니터에 플러그중 어느 부분이 켜져있는지 확인할 수 있으며
시간을 설정할 수 있어 얼마나 남았는지 볼 수 있게 나타낸다.
 
