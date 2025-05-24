# Test-GetAudioDevice.ps1
$exePath = ".\Get-AudioDevice.exe"
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
        Name = "Default (Enabled Output Devices)"
        Args = @()
        Expected = @(
            "Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)",
            "NOVATEK (HD Audio Driver for Display Audio)",
            "Speakers (Realtek(R) Audio)"
        )
    },
    @{
        Name = "Help Option (-h)"
        Args = @("-h")
        Expected = @(
            "Get-AudioDevice.exe",
            "  -h: Display Help Message",
            "  -input or -output displays input or output devices respectively.",
            "  -disabled or -enabled displays disabled or enabled devices respectively.",
            "  Running the executable with no options will display enabled output devices."
        )
    },
    @{
        Name = "Input Devices (-input)"
        Args = @("-input")
        Expected = @(
            "Microphone Array (Intel« Smart Sound Technology for Digital Microphones)",
            "Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)"
        )
    },
    @{
        Name = "Disabled Devices (-disabled)"
        Args = @("-disabled")
        Expected = @()
    },
    @{
        Name = "Disabled Input Devices (-disabled -input)"
        Args = @("-disabled", "-input")
        Expected = @("Stereo Mix (Realtek(R) Audio)")
    },
    @{
        Name = "Output Devices (-output)"
        Args = @("-output")
        Expected = @(
            "Echo Cancelling Speakerphone (Sennheiser BTD 800 USB for Lync)",
            "NOVATEK (HD Audio Driver for Display Audio)",
            "Speakers (Realtek(R) Audio)"
        )
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