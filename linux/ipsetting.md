Ubuntu 22.04에서 여러 랜카드에 고정 IP를 할당하는 방법은 주로 **Netplan**을 사용하여 설정할 수 있습니다. Netplan은 Ubuntu 18.04부터 기본 네트워크 관리 도구로 사용되며, YAML 형식의 구성 파일을 통해 네트워크 설정을 관리합니다. 아래는 각 랜카드별로 디바이스 아이디에 고정 IP를 할당하는 방법입니다.

## Netplan을 이용한 고정 IP 설정

1. **네트워크 인터페이스 확인**
   - 터미널에서 `ip a` 또는 `ip link` 명령어를 사용하여 네트워크 인터페이스 목록을 확인하고, 설정할 인터페이스 이름을 확인합니다.

2. **Netplan 구성 파일 백업**
   - Netplan 구성 파일은 `/etc/netplan/` 디렉토리에 위치합니다. 예를 들어 `01-network-manager-all.yaml` 파일이 있을 수 있습니다.
   - 기존 파일을 백업합니다:
     ```bash
     sudo cp /etc/netplan/01-network-manager-all.yaml /etc/netplan/01-network-manager-all_backup.yaml
     ```

3. **구성 파일 편집**
   - 텍스트 편집기를 사용하여 Netplan 구성 파일을 엽니다:
     ```bash
     sudo nano /etc/netplan/01-network-manager-all.yaml
     ```
   - YAML 형식으로 각 네트워크 인터페이스에 대해 고정 IP를 설정합니다. 예시:
     ```yaml
     network:
       version: 2
       renderer: networkd
       ethernets:
         eth0:
           dhcp4: no
           addresses:
             - 192.168.1.10/24
           gateway4: 192.168.1.1
           nameservers:
             addresses: [8.8.8.8, 8.8.4.4]
         eth1:
           dhcp4: no
           addresses:
             - 192.168.1.11/24
           gateway4: 192.168.1.1
           nameservers:
             addresses: [8.8.8.8, 8.8.4.4]
     ```
   - 각 `ethernets` 아래에 인터페이스 이름(예: `eth0`, `eth1`)과 원하는 IP 주소 및 기타 네트워크 정보를 입력합니다.

4. **설정 적용**
   - 변경 사항을 저장하고 종료한 후, 아래 명령어로 설정을 적용합니다:
     ```bash
     sudo netplan apply
     ```

5. **설정 확인**
   - `ip addr` 명령어를 사용하여 IP 주소가 올바르게 설정되었는지 확인합니다.

이 방법을 통해 각 랜카드별로 고정 IP를 할당할 수 있습니다[1][3][4]. Netplan의 YAML 구문은 들여쓰기에 민감하므로 주의해서 작성해야 합니다[2].

출처
[1] How To Configure Static IP Address In Ubuntu 22.04 (Easy Guide) https://ostechnix.com/configure-static-ip-address-ubuntu/
[2] Ubuntu 22.04 LTS IP설정 방법(고정 IP 설정) - IT-SVR https://it-svr.com/ubuntu-22-04-lts-static-ip/
[3] Configuring networks - Ubuntu https://ubuntu.com/server/docs/configuring-networks
[4] 우분투 고정 IP 설정 Ubuntu Server 22.04 - IT너구리 - 티스토리 https://it-racoon.tistory.com/3
