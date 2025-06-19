$exePath = ".\Get-DefaultAudioDevice.exe"
$baseDir = "C:\github\AudioDeviceManager\x64\Debug"
$passed = 0
$total = 0

$exePath = Join-Path -Path $baseDir -ChildPath $exePath


# Helper function to run a test
function Run-Test {
    param (
        [string]$TestName,
        [string[]]$Arguments,
        [string[]]$ExpectedOutput
    )
    Write-Host "Running test: $TestName"

    # Run the command and capture output
    $output = & $exePath @Arguments | Out-String -Stream # | ForEach-Object { $_.Trim() } | Where-Object { $_ }
    
    # Compare output with expected
    $match = $true
    if ($output.Count -ne $ExpectedOutput.Count) {
        $match = $false
    } else {
        #Special Handling here for commands with a single line of expected output vs. multi-line output
        if ($ExpectedOutput.Count -eq 1) {
            if ($output -ne $ExpectedOutput[0]) {
                $match = $false
            }
        } else {
            for ($i = 0; $i -lt $output.Count; $i++) {
                if ($output[$i] -ne $ExpectedOutput[$i]) {
                    $match = $false
                    break
                }
            }
        }
    }

    # Report result
    if ($match) {
        Write-Host "PASS: $TestName" -ForegroundColor Green
        return $true
    } else {
        $seperator = """`n"""
        Write-Host "FAIL: $TestName" -ForegroundColor Red
        Write-Host "Expected:`n""$($ExpectedOutput -join $seperator)"""
        Write-Host "Got:`n""$($output -join $seperator )"""
        return $false
    }
}

# Define test cases
$tests = @(
    @{
        Name = "Default"
        Args = @()
        Expected = @("Speakers (Realtek(R) Audio)"
        )
    },
    @{
        Name = "Help Option (-h)"
        Args = @("-h")
        Expected = @(
" Get-DefaultAudioDevice",
"  Displays the Default audio output device.",
"   -h : Displays this help",
"   -sounds | -communications : Select the `"role`" to query",
"     sounds : Games, system notifications, Music, movies, narration, and live music recording.",
"     communications : Voice communications (talking to another person).",
"   -Input | -Output",
"     Displays the Input or Output device respectively.",
"  Running the executable with no options displays the default Multimedia output device.",
""       )
    },
    @{
        Name = "(-input)"
        Args = @("-input")
        Expected = @(
            "Microphone Array (Intel« Smart Sound Technology for Digital Microphones)"
        )
    },
    @{
        Name = "(-input -communications)"
        Args = @("-input -communications")
        Expected = @("Speakers (Realtek(R) Audio)")
    },
    @{
        Name = "-output"
        Args = @("-output -sound")
        Expected = @("Speakers (Realtek(R) Audio)")
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