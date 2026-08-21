const SCIENTIFIC_API =
    "/api/evaluate";


const PROGRAMMER_API =
    "/api/programmer";


// ============================================================
// STORAGE
// ============================================================

const STORAGE_KEYS =
{
    angleMode:
        "scientificCalculatorAngleMode",

    memory:
        "scientificCalculatorMemory",

    lastResult:
        "scientificCalculatorLastResult",

    history:
        "scientificCalculatorHistory",

    theme:
        "scientificCalculatorTheme",

    calculatorMode:
        "scientificCalculatorMode"
};


const MAX_HISTORY_ITEMS =
    20;


// ============================================================
// SCIENTIFIC STATE
// ============================================================

let angleMode =
    sessionStorage.getItem(
        STORAGE_KEYS.angleMode
    ) || "DEG";


let memoryValue =
    Number(
        sessionStorage.getItem(
            STORAGE_KEYS.memory
        ) || "0"
    );


let lastResult =
    getStoredNumber(
        STORAGE_KEYS.lastResult
    );


let calculationHistory =
    loadHistory();


let historyCollapsed =
    false;


// ============================================================
// APPLICATION STATE
// ============================================================

let theme =
    localStorage.getItem(
        STORAGE_KEYS.theme
    ) || "dark";


let calculatorMode =
    sessionStorage.getItem(
        STORAGE_KEYS.calculatorMode
    ) || "scientific";


// ============================================================
// PROGRAMMER STATE
// ============================================================

let programmerBase =
    "DEC";


let programmerValue =
    "0";


let programmerLeft =
    null;


let programmerPendingOperation =
    null;


let programmerWaitingForRight =
    false;


let programmerConversions =
{
    BIN:
        "0",

    OCT:
        "0",

    DEC:
        "0",

    HEX:
        "0"
};


// ============================================================
// DOM
// ============================================================

const scientificMode =
    document.getElementById(
        "scientificMode"
    );


const programmerMode =
    document.getElementById(
        "programmerMode"
    );


const scientificModeButton =
    document.getElementById(
        "scientificModeButton"
    );


const programmerModeButton =
    document.getElementById(
        "programmerModeButton"
    );


const engineSubtitle =
    document.getElementById(
        "engineSubtitle"
    );


const themeButton =
    document.getElementById(
        "themeButton"
    );


const modeButton =
    document.getElementById(
        "modeButton"
    );


const display =
    document.getElementById(
        "display"
    );


const preview =
    document.getElementById(
        "expressionPreview"
    );


const errorElement =
    document.getElementById(
        "error"
    );


const statusMessage =
    document.getElementById(
        "statusMessage"
    );


const expressionInput =
    document.getElementById(
        "expressionInput"
    );


const displayMode =
    document.getElementById(
        "displayMode"
    );


const expressionModeLabel =
    document.getElementById(
        "expressionModeLabel"
    );


const memoryIndicator =
    document.getElementById(
        "memoryIndicator"
    );


const historyList =
    document.getElementById(
        "historyList"
    );


const historyCount =
    document.getElementById(
        "historyCount"
    );


const toggleHistoryButton =
    document.getElementById(
        "toggleHistoryButton"
    );


const equalsButton =
    document.getElementById(
        "equalsButton"
    );


// Programmer DOM

const programmerBaseBadge =
    document.getElementById(
        "programmerBaseBadge"
    );


const programmerExpression =
    document.getElementById(
        "programmerExpression"
    );


const programmerMainValue =
    document.getElementById(
        "programmerMainValue"
    );


const programmerError =
    document.getElementById(
        "programmerError"
    );


const programmerStatus =
    document.getElementById(
        "programmerStatus"
    );


const conversionBIN =
    document.getElementById(
        "conversionBIN"
    );


const conversionOCT =
    document.getElementById(
        "conversionOCT"
    );


const conversionDEC =
    document.getElementById(
        "conversionDEC"
    );


const conversionHEX =
    document.getElementById(
        "conversionHEX"
    );


// ============================================================
// STORAGE HELPERS
// ============================================================

function getStoredNumber(key)
{
    const raw =
        sessionStorage.getItem(
            key
        );


    if (
        raw === null ||
        raw === ""
    )
    {
        return null;
    }


    const value =
        Number(raw);


    return Number.isFinite(value)
        ? value
        : null;
}


function loadHistory()
{
    try
    {
        const raw =
            sessionStorage.getItem(
                STORAGE_KEYS.history
            );


        if (!raw)
        {
            return [];
        }


        const parsed =
            JSON.parse(raw);


        return Array.isArray(parsed)
            ? parsed.slice(
                0,
                MAX_HISTORY_ITEMS
            )
            : [];
    }

    catch
    {
        return [];
    }
}


function saveHistory()
{
    sessionStorage.setItem(
        STORAGE_KEYS.history,
        JSON.stringify(
            calculationHistory
        )
    );
}


// ============================================================
// THEME
// ============================================================

function applyTheme()
{
    document.documentElement.setAttribute(
        "data-theme",
        theme
    );


    themeButton.textContent =
        theme === "dark"
            ? "☾"
            : "☀";
}


function toggleTheme()
{
    theme =
        theme === "dark"
            ? "light"
            : "dark";


    localStorage.setItem(
        STORAGE_KEYS.theme,
        theme
    );


    applyTheme();
}


// ============================================================
// CALCULATOR MODE
// ============================================================

function switchCalculatorMode(mode)
{
    calculatorMode =
        mode;


    sessionStorage.setItem(
        STORAGE_KEYS.calculatorMode,
        calculatorMode
    );


    const scientific =
        calculatorMode ===
        "scientific";


    scientificMode.classList.toggle(
        "hidden",
        !scientific
    );


    programmerMode.classList.toggle(
        "hidden",
        scientific
    );


    scientificModeButton.classList.toggle(
        "active",
        scientific
    );


    programmerModeButton.classList.toggle(
        "active",
        !scientific
    );


    modeButton.classList.toggle(
        "hidden",
        !scientific
    );


    engineSubtitle.textContent =
        scientific
            ? "Scientific Expression Engine"
            : "64-bit C++ Programmer Engine";


    if (!scientific)
    {
        updateProgrammerDisplay();

        refreshProgrammerConversions();
    }
}


// ============================================================
// SCIENTIFIC ERROR / STATUS
// ============================================================

function clearError()
{
    errorElement.textContent =
        "";
}


function showError(message)
{
    errorElement.textContent =
        message;
}


function showStatus(message)
{
    statusMessage.textContent =
        message;


    setTimeout(
        function()
        {
            if (
                statusMessage.textContent ===
                message
            )
            {
                statusMessage.textContent =
                    "";
            }
        },
        1800
    );
}


// ============================================================
// ANGLE MODE
// ============================================================

function updateAngleModeInterface()
{
    modeButton.textContent =
        angleMode;


    displayMode.textContent =
        angleMode;


    const rad =
        angleMode ===
        "RAD";


    modeButton.classList.toggle(
        "rad",
        rad
    );


    expressionModeLabel.textContent =
        rad
            ? "Trigonometry: Radians"
            : "Trigonometry: Degrees";
}


function toggleAngleMode()
{
    angleMode =
        angleMode === "DEG"
            ? "RAD"
            : "DEG";


    sessionStorage.setItem(
        STORAGE_KEYS.angleMode,
        angleMode
    );


    updateAngleModeInterface();

    updatePreview();
}


// ============================================================
// MEMORY
// ============================================================

function updateMemoryIndicator()
{
    memoryIndicator.textContent =
        Math.abs(memoryValue) <
        1e-15
            ? ""
            : `M: ${formatNumber(memoryValue)}`;
}


function saveMemory()
{
    sessionStorage.setItem(
        STORAGE_KEYS.memory,
        String(memoryValue)
    );


    updateMemoryIndicator();
}


function memoryClear()
{
    memoryValue =
        0;


    saveMemory();

    showStatus(
        "Memory cleared."
    );
}


function memoryRecall()
{
    appendNumericValue(
        memoryValue
    );
}


function memoryAdd()
{
    if (
        lastResult ===
        null
    )
    {
        showError(
            "Calculate a result before using M+."
        );

        return;
    }


    memoryValue +=
        lastResult;


    saveMemory();

    showStatus(
        "Added to memory."
    );
}


function memorySubtract()
{
    if (
        lastResult ===
        null
    )
    {
        showError(
            "Calculate a result before using M−."
        );

        return;
    }


    memoryValue -=
        lastResult;


    saveMemory();

    showStatus(
        "Subtracted from memory."
    );
}


function appendAnswer()
{
    if (
        lastResult ===
        null
    )
    {
        showError(
            "No previous answer."
        );

        return;
    }


    appendNumericValue(
        lastResult
    );
}


// ============================================================
// SCIENTIFIC INPUT
// ============================================================

function expressionNeedsMultiplicationBeforeValue()
{
    const text =
        expressionInput.value.trimEnd();


    if (!text)
    {
        return false;
    }


    return /[0-9A-Za-z)!]$/.test(
        text
    );
}


function appendNumericValue(value)
{
    let text =
        serializeNumber(
            value
        );


    if (value < 0)
    {
        text =
            `(${text})`;
    }


    if (
        expressionNeedsMultiplicationBeforeValue()
    )
    {
        expressionInput.value +=
            "*";
    }


    expressionInput.value +=
        text;


    updatePreview();
}


function appendToken(token)
{
    clearError();


    expressionInput.value +=
        token;


    updatePreview();
}


function appendNumber(number)
{
    appendToken(
        number
    );
}


function appendDecimal()
{
    appendToken(
        "."
    );
}


function appendOperator(operator)
{
    appendToken(
        operator
    );
}


function appendConstant(constant)
{
    if (
        expressionNeedsMultiplicationBeforeValue()
    )
    {
        expressionInput.value +=
            "*";
    }


    expressionInput.value +=
        constant;


    updatePreview();
}


function appendFunction(name)
{
    if (
        expressionNeedsMultiplicationBeforeValue()
    )
    {
        expressionInput.value +=
            "*";
    }


    expressionInput.value +=
        `${name}(`;


    updatePreview();
}


function appendSquare()
{
    appendToken(
        "^2"
    );
}


function appendCube()
{
    appendToken(
        "^3"
    );
}


function appendFactorial()
{
    appendToken(
        "!"
    );
}


function toggleSignExpression()
{
    const expression =
        expressionInput.value.trim();


    if (!expression)
    {
        appendToken(
            "-"
        );

        return;
    }


    expressionInput.value =
        `-(${expression})`;


    updatePreview();
}


function backspace()
{
    expressionInput.value =
        expressionInput.value.slice(
            0,
            -1
        );


    updatePreview();
}


function clearCalculator()
{
    expressionInput.value =
        "";


    display.textContent =
        "0";


    preview.textContent =
        "Ready";


    clearError();
}


function updatePreview()
{
    preview.textContent =
        expressionInput.value
            ? `${expressionInput.value} [${angleMode}]`
            : "Ready";
}


// ============================================================
// SCIENTIFIC EVALUATION
// ============================================================

async function evaluateExpression()
{
    clearError();


    const expression =
        expressionInput.value.trim();


    if (!expression)
    {
        showError(
            "Enter an expression."
        );

        return;
    }


    equalsButton.textContent =
        "…";


    try
    {
        const response =
            await fetch(
                SCIENTIFIC_API,
                {
                    method:
                        "POST",

                    headers:
                    {
                        "Content-Type":
                            "application/json"
                    },

                    body:
                        JSON.stringify(
                            {
                                expression:
                                    expression,

                                mode:
                                    angleMode
                            }
                        )
                }
            );


        const data =
            await response.json();


        if (
            !response.ok ||
            data.error
        )
        {
            throw new Error(
                data.error ||
                "Calculation failed."
            );
        }


        lastResult =
            Number(
                data.result
            );


        sessionStorage.setItem(
            STORAGE_KEYS.lastResult,
            String(lastResult)
        );


        display.textContent =
            formatNumber(
                lastResult
            );


        preview.textContent =
            `${expression} [${data.mode}] =`;


        addHistoryItem(
            expression,
            lastResult,
            data.mode
        );
    }

    catch (error)
    {
        showError(
            error.message
        );
    }

    finally
    {
        equalsButton.textContent =
            "=";
    }
}


// ============================================================
// HISTORY
// ============================================================

function addHistoryItem(
    expression,
    result,
    mode
)
{
    calculationHistory.unshift(
        {
            expression:
                expression,

            result:
                result,

            mode:
                mode
        }
    );


    calculationHistory =
        calculationHistory.slice(
            0,
            MAX_HISTORY_ITEMS
        );


    saveHistory();

    renderHistory();
}


function renderHistory()
{
    historyList.innerHTML =
        "";


    historyCount.textContent =
        calculationHistory.length === 1
            ? "1 calculation"
            : `${calculationHistory.length} calculations`;


    if (
        calculationHistory.length ===
        0
    )
    {
        const empty =
            document.createElement(
                "div"
            );


        empty.className =
            "history-empty";


        empty.textContent =
            "No calculations yet.";


        historyList.appendChild(
            empty
        );


        return;
    }


    calculationHistory.forEach(
        function(item)
        {
            const button =
                document.createElement(
                    "button"
                );


            button.className =
                "history-item";


            button.innerHTML =
                `
                    <span class="history-expression"></span>

                    <span class="history-result-row">

                        <span class="history-result"></span>

                        <span class="history-mode"></span>

                    </span>
                `;


            button.querySelector(
                ".history-expression"
            ).textContent =
                item.expression;


            button.querySelector(
                ".history-result"
            ).textContent =
                `= ${formatNumber(item.result)}`;


            button.querySelector(
                ".history-mode"
            ).textContent =
                item.mode;


            button.onclick =
                function()
                {
                    expressionInput.value =
                        item.expression;


                    angleMode =
                        item.mode;


                    lastResult =
                        item.result;


                    updateAngleModeInterface();

                    updatePreview();


                    display.textContent =
                        formatNumber(
                            item.result
                        );
                };


            historyList.appendChild(
                button
            );
        }
    );
}


function clearHistory()
{
    calculationHistory =
        [];


    sessionStorage.removeItem(
        STORAGE_KEYS.history
    );


    renderHistory();
}


function toggleHistory()
{
    historyCollapsed =
        !historyCollapsed;


    historyList.classList.toggle(
        "collapsed",
        historyCollapsed
    );


    toggleHistoryButton.textContent =
        historyCollapsed
            ? "Expand"
            : "Collapse";
}


// ============================================================
// COPY
// ============================================================

async function copyExpression()
{
    const value =
        expressionInput.value.trim();


    if (!value)
    {
        showError(
            "There is no expression to copy."
        );

        return;
    }


    try
    {
        await navigator.clipboard.writeText(
            value
        );


        showStatus(
            "Expression copied."
        );
    }

    catch
    {
        showError(
            "Could not copy expression."
        );
    }
}


async function copyResult()
{
    if (
        lastResult ===
        null
    )
    {
        showError(
            "There is no result to copy."
        );

        return;
    }


    try
    {
        await navigator.clipboard.writeText(
            serializeNumber(
                lastResult
            )
        );


        showStatus(
            "Result copied."
        );
    }

    catch
    {
        showError(
            "Could not copy result."
        );
    }
}


// ============================================================
// PROGRAMMER HELPERS
// ============================================================

function clearProgrammerError()
{
    programmerError.textContent =
        "";
}


function showProgrammerError(message)
{
    programmerError.textContent =
        message;
}


function showProgrammerStatus(message)
{
    programmerStatus.textContent =
        message;


    setTimeout(
        function()
        {
            if (
                programmerStatus.textContent ===
                message
            )
            {
                programmerStatus.textContent =
                    "";
            }
        },
        1500
    );
}


function programmerDigitAllowed(digit)
{
    const value =
        parseInt(
            digit,
            16
        );


    switch (programmerBase)
    {
        case "BIN":
            return value <= 1;

        case "OCT":
            return value <= 7;

        case "DEC":
            return value <= 9;

        case "HEX":
            return value <= 15;

        default:
            return false;
    }
}


function updateProgrammerKeyAvailability()
{
    const digits =
        [
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "A",
            "B",
            "C",
            "D",
            "E",
            "F"
        ];


    digits.forEach(
        function(digit)
        {
            const id =
                /^[A-F]$/.test(digit)
                    ? `hex${digit}`
                    : `prog${digit}`;


            const element =
                document.getElementById(
                    id
                );


            if (element)
            {
                element.disabled =
                    !programmerDigitAllowed(
                        digit
                    );
            }
        }
    );
}


function updateProgrammerDisplay()
{
    programmerMainValue.textContent =
        programmerValue;


    programmerBaseBadge.textContent =
        programmerBase;


    document
        .querySelectorAll(
            ".base-button"
        )
        .forEach(
            function(button)
            {
                button.classList.remove(
                    "active"
                );
            }
        );


    document.getElementById(
        `base${programmerBase}`
    ).classList.add(
        "active"
    );


    updateProgrammerKeyAvailability();
}


function appendProgrammerDigit(digit)
{
    clearProgrammerError();


    if (
        !programmerDigitAllowed(
            digit
        )
    )
    {
        return;
    }


    if (programmerWaitingForRight)
    {
        programmerValue =
            digit;


        programmerWaitingForRight =
            false;
    }

    else if (
        programmerValue ===
        "0"
    )
    {
        programmerValue =
            digit;
    }

    else
    {
        programmerValue +=
            digit;
    }


    updateProgrammerDisplay();

    refreshProgrammerConversions();
}


function programmerBackspace()
{
    programmerValue =
        programmerValue.length <= 1
            ? "0"
            : programmerValue.slice(
                0,
                -1
            );


    updateProgrammerDisplay();

    refreshProgrammerConversions();
}


function clearProgrammer()
{
    programmerValue =
        "0";


    programmerLeft =
        null;


    programmerPendingOperation =
        null;


    programmerWaitingForRight =
        false;


    programmerExpression.textContent =
        "Ready";


    clearProgrammerError();

    updateProgrammerDisplay();

    refreshProgrammerConversions();
}


// ============================================================
// PROGRAMMER API
// ============================================================

async function programmerRequest(payload)
{
    const response =
        await fetch(
            PROGRAMMER_API,
            {
                method:
                    "POST",

                headers:
                {
                    "Content-Type":
                        "application/json"
                },

                body:
                    JSON.stringify(
                        payload
                    )
            }
        );


    const data =
        await response.json();


    if (
        !response.ok ||
        data.error
    )
    {
        throw new Error(
            data.error ||
            "Programmer calculation failed."
        );
    }


    return data;
}


async function refreshProgrammerConversions()
{
    try
    {
        const data =
            await programmerRequest(
                {
                    operation:
                        "CONVERT",

                    left:
                        programmerValue,

                    base:
                        programmerBase
                }
            );


        programmerConversions =
        {
            BIN:
                data.bin,

            OCT:
                data.oct,

            DEC:
                data.dec,

            HEX:
                data.hex
        };


        conversionBIN.textContent =
            data.bin;


        conversionOCT.textContent =
            data.oct;


        conversionDEC.textContent =
            data.dec;


        conversionHEX.textContent =
            data.hex;
    }

    catch (error)
    {
        showProgrammerError(
            error.message
        );
    }
}


// ============================================================
// PROGRAMMER BASE
// ============================================================

async function setProgrammerBase(base)
{
    clearProgrammerError();


    try
    {
        const data =
            await programmerRequest(
                {
                    operation:
                        "CONVERT",

                    left:
                        programmerValue,

                    base:
                        programmerBase
                }
            );


        programmerBase =
            base;


        switch (base)
        {
            case "BIN":
                programmerValue =
                    data.bin;
                break;

            case "OCT":
                programmerValue =
                    data.oct;
                break;

            case "DEC":
                programmerValue =
                    data.dec;
                break;

            case "HEX":
                programmerValue =
                    data.hex;
                break;
        }


        updateProgrammerDisplay();

        await refreshProgrammerConversions();
    }

    catch (error)
    {
        showProgrammerError(
            error.message
        );
    }
}


// ============================================================
// PROGRAMMER OPERATIONS
// ============================================================

function chooseProgrammerOperation(operation)
{
    clearProgrammerError();


    programmerLeft =
        programmerValue;


    programmerPendingOperation =
        operation;


    programmerWaitingForRight =
        true;


    programmerExpression.textContent =
        `${programmerLeft} ${programmerOperatorLabel(operation)}`;
}


function programmerOperatorLabel(operation)
{
    const labels =
    {
        AND:
            "AND",

        OR:
            "OR",

        XOR:
            "XOR",

        SHL:
            "<<",

        SHR:
            ">>"
    };


    return labels[operation] ||
        operation;
}


async function executeProgrammerOperation()
{
    clearProgrammerError();


    if (
        programmerPendingOperation ===
        null ||
        programmerLeft ===
        null
    )
    {
        await refreshProgrammerConversions();

        return;
    }


    try
    {
        const right =
            programmerValue;


        const data =
            await programmerRequest(
                {
                    operation:
                        programmerPendingOperation,

                    left:
                        programmerLeft,

                    right:
                        right,

                    base:
                        programmerBase
                }
            );


        programmerExpression.textContent =
            `${programmerLeft} ${programmerOperatorLabel(programmerPendingOperation)} ${right} =`;


        programmerValue =
            valueForProgrammerBase(
                data
            );


        programmerLeft =
            null;


        programmerPendingOperation =
            null;


        programmerWaitingForRight =
            true;


        updateConversionsFromResponse(
            data
        );


        updateProgrammerDisplay();
    }

    catch (error)
    {
        showProgrammerError(
            error.message
        );
    }
}


async function performProgrammerNot()
{
    clearProgrammerError();


    try
    {
        const original =
            programmerValue;


        const data =
            await programmerRequest(
                {
                    operation:
                        "NOT",

                    left:
                        original,

                    base:
                        programmerBase
                }
            );


        programmerExpression.textContent =
            `NOT ${original} =`;


        programmerValue =
            valueForProgrammerBase(
                data
            );


        programmerWaitingForRight =
            true;


        updateConversionsFromResponse(
            data
        );


        updateProgrammerDisplay();
    }

    catch (error)
    {
        showProgrammerError(
            error.message
        );
    }
}


function valueForProgrammerBase(data)
{
    switch (programmerBase)
    {
        case "BIN":
            return data.bin;

        case "OCT":
            return data.oct;

        case "HEX":
            return data.hex;

        default:
            return data.dec;
    }
}


function updateConversionsFromResponse(data)
{
    programmerConversions =
    {
        BIN:
            data.bin,

        OCT:
            data.oct,

        DEC:
            data.dec,

        HEX:
            data.hex
    };


    conversionBIN.textContent =
        data.bin;


    conversionOCT.textContent =
        data.oct;


    conversionDEC.textContent =
        data.dec;


    conversionHEX.textContent =
        data.hex;
}


// ============================================================
// NUMBER FORMATTING
// ============================================================

function serializeNumber(number)
{
    if (!Number.isFinite(number))
    {
        return "0";
    }


    return String(
        Number(
            number.toPrecision(15)
        )
    );
}


function formatNumber(number)
{
    if (!Number.isFinite(number))
    {
        return "Error";
    }


    if (
        Math.abs(number) <
        1e-12
    )
    {
        return "0";
    }


    if (
        Math.abs(number) >=
            1e12 ||
        Math.abs(number) <
            1e-9
    )
    {
        return number.toExponential(
            10
        );
    }


    return Number(
        number.toPrecision(12)
    ).toString();
}


// ============================================================
// SCIENTIFIC INPUT EVENTS
// ============================================================

expressionInput.addEventListener(
    "input",

    function()
    {
        clearError();

        updatePreview();
    }
);


expressionInput.addEventListener(
    "keydown",

    async function(event)
    {
        if (
            event.key ===
            "Enter"
        )
        {
            event.preventDefault();

            await evaluateExpression();
        }


        if (
            event.key ===
            "Escape"
        )
        {
            clearCalculator();
        }
    }
);


// ============================================================
// INITIALIZATION
// ============================================================

applyTheme();

updateAngleModeInterface();

updateMemoryIndicator();

renderHistory();

updatePreview();

updateProgrammerDisplay();

refreshProgrammerConversions();

switchCalculatorMode(
    calculatorMode
);


if (
    lastResult !==
    null
)
{
    display.textContent =
        formatNumber(
            lastResult
        );
}
