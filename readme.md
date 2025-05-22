# AudioDeviceManager 

This is a collection of command-line utilities for managing Audio Devices in Windows, to automate the functions provided by the Sound Control panel, mmsys.cpl..  They can list devices, set the default devices, and enable/disable devices.

Replace this:

![MMsys.Cpl Screenshot](doc/mmsyscpl.png)

With this:

![Screenshot](doc/screenshot.png)

## Utilities
```
Get-AudioDevice.exe
  -h: Display Help Message
  -input or -output displays input or output devices respectively.
  -disabled or -enabled displays disabled or enabled devices respectively.
  Running the executable with no options will display enabled output devices.

set-defaultaudiodevice - Sets the default audio output device.
  Example: Set-DefaultAudioDevice "Speakers (Realtek High Definition Audio)"
  Substring matches are supported, e.g. Set-DefaultAudioDevice "polycom" 

enable-audiodevice - Enables the specified audio device.
  Example: Enable-AudioDevice "Speakers (Realtek High Definition Audio)"
  Substring matches are supported, e.g. Enable-AudioDevice "polycom"


disable-audiodevice - Disables the specified audio device.
  Example: Disable-AudioDevice "Speakers (Realtek High Definition Audio)"
  Substring matches are supported, e.g. Disable-AudioDevice "polycom"
```

## Disclaimer

Reiterating the terms of the MIT release license:

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
