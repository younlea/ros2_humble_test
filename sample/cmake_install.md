Ubuntu 22.04에서 CMake 3.30 이상을 설치하는 방법은 여러 가지가 있습니다. 여기서는 Snap 패키지와 소스 코드를 통해 설치하는 방법을 설명하겠습니다.

### 방법 1: Snap 패키지를 사용하여 설치

Snap은 Ubuntu에서 최신 버전의 소프트웨어를 쉽게 설치할 수 있는 방법입니다. Snap을 사용하여 CMake를 설치하면 최신 버전을 얻을 수 있습니다.

1. **패키지 목록 업데이트**:
   ```bash
   sudo apt update
   ```

2. **CMake 설치**:
   ```bash
   sudo snap install cmake --classic
   ```

3. **설치 확인**:
   ```bash
   cmake --version
   ```

Snap을 사용하면 최신 버전의 CMake를 쉽게 설치할 수 있으며, 이 방법은 간단하고 빠릅니다[1][5].

### 방법 2: 소스 코드에서 직접 설치

CMake의 최신 버전을 소스 코드에서 직접 빌드하여 설치할 수도 있습니다.

1. **필요한 패키지 설치**:
   ```bash
   sudo apt install build-essential libssl-dev
   ```

2. **소스 코드 다운로드**:
   CMake의 최신 버전을 다운로드합니다. 예를 들어, CMake 3.30.2를 설치하려면 다음 명령어를 사용합니다.
   ```bash
   wget https://cmake.org/files/v3.30/cmake-3.30.2.tar.gz
   ```

3. **압축 해제**:
   ```bash
   tar -xzvf cmake-3.30.2.tar.gz
   ```

4. **디렉토리 이동 및 설치**:
   ```bash
   cd cmake-3.30.2
   ./bootstrap
   make -j$(nproc)
   sudo make install
   ```

5. **설치 확인**:
   ```bash
   cmake --version
   ```

이 방법은 최신 버전을 설치할 수 있는 확실한 방법이지만, 빌드 과정이 다소 시간이 걸릴 수 있습니다[2][6][7].

위의 두 가지 방법 중 하나를 선택하여 CMake 3.30 이상의 버전을 Ubuntu 22.04에 설치할 수 있습니다. Snap을 사용하는 방법이 더 간단하고 빠르지만, 소스 코드에서 직접 설치하는 방법은 최신 버전을 보장합니다.

출처
[1] How to Install CMake on Ubuntu 22.04 LTS | LinuxSimply - YouTube https://www.youtube.com/watch?v=IWyzYzyLVXs
[2] How to Install CMake on Ubuntu 22.04 LTS - VITUX https://vitux.com/how-to-install-cmake-on-ubuntu/
[3] How to reinstall the latest cmake version? - Stack Overflow https://stackoverflow.com/questions/49859457/how-to-reinstall-the-latest-cmake-version
[4] Installing CMake https://cliutils.gitlab.io/modern-cmake/chapters/intro/installing.html
[5] Install CMake on Linux | Snap Store - Snapcraft https://snapcraft.io/cmake
[6] [CMake] Ubuntu에 CMake 설치하기 - 꾸준희 - 티스토리 https://eehoeskrap.tistory.com/397
[7] Install CMake 3.22 Ubuntu 22.04 - GitHub Gist https://gist.github.com/UbuntuEvangelist/afd13e6fba7ffc5dbf7c5da31b55dff6
[8] Ubuntu에서 CMake 설치 방법 - 몽구의 우당탕탕 개발 공부 - 티스토리 https://mong9data.tistory.com/124
