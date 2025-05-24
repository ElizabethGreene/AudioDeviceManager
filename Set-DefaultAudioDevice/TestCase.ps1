
$exePath = ".\Set-DefaultAudioDevice.exe"
$baseDir = "C:\github\AudioDeviceManager\x64\Debug"
$passed = 0
$total = 0

# Change to the directory containing the executable
Set-Location -Path $baseDir

# Helper function to run a test
function Run-Test {
    param (
        [string]$TestName,
        [string[]]$Arguments,
        [string[]]$ExpectedOutput
    )
    Write-Host "Running test: $TestName"

    # Run the command and capture output
    $output = & $exePath @Arguments | Out-String -Stream | ForEach-Object { $_.Trim() } | Where-Object { $_ }
    
    # Compare output with expected
    $match = $true
    if ($output.Count -ne $ExpectedOutput.Count) {
        $match = $false
    } else {
        for ($i = 0; $i -lt $output.Count; $i++) {
            if ($output[$i] -ne $ExpectedOutput[$i]) {
                $match = $false
                break
            }
        }
    }

    # Report result
    if ($match) {
        Write-Host "PASS: $TestName" -ForegroundColor Green
        return $true
    } else {
        Write-Host "FAIL: $TestName" -ForegroundColor Red
        Write-Host "Expected: ---$($ExpectedOutput -join ', ')---"
        Write-Host "Got: ---$($output -join ', ')---"
        return $false
    }
}

# Define test cases
$tests = @(
    @{
        Name = "Default (no options)"
        Args = @()
        Expected = @(
"No device name found"
" Set-DefaultAudioDevice [-h] [-input|-output] [-sound|-communications] <device_name>"
"  Sets the Default sound or communications device."
""
" Syntax:"
"  -h (or with no options) : Displays this help"
"  -sounds | -communications : Select the ""role"" to set"
"    -sounds : Games, system notifications, Music, movies, narration, and live music recording."
"    -communications : Voice communications (talking to another person)."
"  -Input | -Output"
"    Sets the Input or Output device respectively."
"  <device name> : The name of the audio device to set as default."
"    Substring matches are supported, e.g. ""polycom"" will match to the first device with polycom in the name."
""
"  Running the executable with just a device name sets the default sound output device."
"    Example: Set-DefaultAudioDevice ""Realtek"""
        )
    },
    @{
        Name = "Help Option (-h)"
        Args = @("-h")
        Expected = @(
"No device name found"
" Set-DefaultAudioDevice [-h] [-input|-output] [-sound|-communications] <device_name>"
"  Sets the Default sound or communications device."
""
" Syntax:"
"  -h (or with no options) : Displays this help"
"  -sounds | -communications : Select the ""role"" to set"
"    -sounds : Games, system notifications, Music, movies, narration, and live music recording."
"    -communications : Voice communications (talking to another person)."
"  -Input | -Output"
"    Sets the Input or Output device respectively."
"  <device name> : The name of the audio device to set as default."
"    Substring matches are supported, e.g. ""polycom"" will match to the first device with polycom in the name."
""
"  Running the executable with just a device name sets the default sound output device."
"    Example: Set-DefaultAudioDevice ""Realtek"""
        )
    },
    @{
        Name = "Input Devices (-input)"
        Args = @("-input","senn")
        Expected = @(
            "Default audio device set to: Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)"          
        )
    },
    @{
        Name = "Output Devices (senn)"
        Args = @("senn")
        Expected = @(
        "Default audio device set to: Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)"
        )
    },
        @{
        Name = "Input Devices (-input -communications)"
        Args = @("-input","-communications","senn")
        Expected = @(
            "Default audio device set to: Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)"          
        )
    },
    @{
        Name = "Output Devices (senn -communications)"
        Args = @("senn"."-communications")
        Expected = @(
        "Default audio device set to: Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)"
        )
    },
    @{
        Name = "Non extant input"
        Args = @("asdfasdf", "-input")
        Expected = @("Failed to find a matching device.")
    },
    @{
        Name = "Non extant output"
        Args = @("asdfasdf")
        Expected = @("Failed to find a matching device.")
    }
)

# Run all tests
foreach ($test in $tests) {
    $total = $total + 1
    $result = Run-Test -TestName $test.Name -Arguments $test.Args -ExpectedOutput $test.Expected
    if ($result) {
        $passed = $passed + 1
    }
}

# Summary
Write-Host "`nTest Summary: $passed/$total tests passed" -ForegroundColor Cyan
if ($passed -eq $total) {
    Write-Host "All tests passed!" -ForegroundColor Green
} else {
    Write-Host "Some tests failed." -ForegroundColor Red
}