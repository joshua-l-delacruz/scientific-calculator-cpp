const API_URL =
    "/api/evaluate";


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
        "scientificCalculatorTheme"
};


const MAX_HISTORY_ITEMS =
    20;


// ============================================================
// STATE
// ============================================================

let angleMode =
    sessionStorage.getItem(
        STORAGE_KEYS.angleMode
    ) || "DEG";


if (
    angleMode !== "DEG" &&
    angleMode !== "RAD"
)
{
    angleMode =
        "DEG";
}


let memoryValue =
    Number(
        sessionStorage.getItem(
            STORAGE_KEYS.memory
        ) || "0"
    );


if (!Number.isFinite(memoryValue))
{
    memoryValue =
        0;
}


let lastResult =
    getStoredNumber(
        STORAGE_KEYS.lastResult
    );


let calculationHistory =
    loadHistory();


let theme =
    localStorage.getItem(
        STORAGE_KEYS.theme
    ) || "dark";


if (
    theme !== "dark" &&
    theme !== "light"
)
{
    theme =
        "dark";
}


let historyCollapsed =
    false;


// ============================================================
// DOM
// ============================================================

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


const modeButton =
    document.getElementById(
        "modeButton"
    );


const themeButton =
    document.getElementById(
        "themeButton"
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


        if (!Array.isArray(parsed))
        {
            return [];
        }


        return parsed
            .filter(
                function(item)
                {
                    return (
                        item &&
                        typeof item.expression ===
                            "string" &&
                        typeof item.result ===
                            "number" &&
                        (
                            item.mode === "DEG" ||
                            item.mode === "RAD"
                        )
                    );
                }
            )
            .slice(
                0,
                MAX_HISTORY_ITEMS
            );
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
// STATUS / ERROR
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


    window.setTimeout(
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
// ANGLE MODE
// ============================================================

function updateAngleModeInterface()
{
    modeButton.textContent =
        angleMode;


    displayMode.textContent =
        angleMode;


    if (angleMode === "RAD")
    {
        modeButton.classList.add(
            "rad"
        );


        expressionModeLabel.textContent =
            "Trigonometry: Radians";
    }

    else
    {
        modeButton.classList.remove(
            "rad"
        );


        expressionModeLabel.textContent =
            "Trigonometry: Degrees";
    }
}


function toggleAngleMode()
{
    clearError();


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

    expressionInput.focus();
}


// ============================================================
// MEMORY
// ============================================================

function saveMemory()
{
    sessionStorage.setItem(
        STORAGE_KEYS.memory,
        String(
            memoryValue
        )
    );


    updateMemoryIndicator();
}


function updateMemoryIndicator()
{
    if (
        Math.abs(memoryValue) <
        1e-15
    )
    {
        memoryIndicator.textContent =
            "";

        return;
    }


    memoryIndicator.textContent =
        `M: ${formatNumber(memoryValue)}`;
}


function memoryClear()
{
    clearError();


    memoryValue =
        0;


    saveMemory();


    showStatus(
        "Memory cleared."
    );


    expressionInput.focus();
}


function memoryRecall()
{
    clearError();


    appendNumericValue(
        memoryValue
    );


    showStatus(
        "Memory recalled."
    );
}


function memoryAdd()
{
    clearError();


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
        "Result added to memory."
    );


    expressionInput.focus();
}


function memorySubtract()
{
    clearError();


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
        "Result subtracted from memory."
    );


    expressionInput.focus();
}


// ============================================================
// ANSWER
// ============================================================

function appendAnswer()
{
    clearError();


    if (
        lastResult ===
        null
    )
    {
        showError(
            "No previous answer is available."
        );

        return;
    }


    appendNumericValue(
        lastResult
    );
}


// ============================================================
// INSERTION HELPERS
// ============================================================

function expressionNeedsMultiplicationBeforeValue()
{
    const expression =
        expressionInput.value.trimEnd();


    if (
        expression.length ===
        0
    )
    {
        return false;
    }


    const lastCharacter =
        expression[
            expression.length - 1
        ];


    return (
        lastCharacter === ')' ||
        lastCharacter === '!' ||
        /[0-9a-zA-Z]/.test(
            lastCharacter
        )
    );
}


function appendNumericValue(value)
{
    const text =
        serializeNumber(
            value
        );


    let valueExpression =
        text;


    if (value < 0)
    {
        valueExpression =
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
        valueExpression;


    updatePreview();

    expressionInput.focus();
}


// ============================================================
// PREVIEW
// ============================================================

function updatePreview()
{
    const expression =
        expressionInput.value;


    if (expression.length > 0)
    {
        preview.textContent =
            `${expression} [${angleMode}]`;
    }

    else
    {
        preview.textContent =
            "Ready";
    }
}


// ============================================================
// APPEND
// ============================================================

function appendToken(token)
{
    clearError();


    expressionInput.value +=
        token;


    updatePreview();

    expressionInput.focus();
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
    clearError();


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

    expressionInput.focus();
}


function appendFunction(functionName)
{
    clearError();


    if (
        expressionNeedsMultiplicationBeforeValue()
    )
    {
        expressionInput.value +=
            "*";
    }


    expressionInput.value +=
        functionName +
        "(";


    updatePreview();

    expressionInput.focus();
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


// ============================================================
// SIGN
// ============================================================

function toggleSignExpression()
{
    clearError();


    const expression =
        expressionInput.value.trim();


    if (expression.length === 0)
    {
        appendToken(
            "-"
        );

        return;
    }


    expressionInput.value =
        "-(" +
        expression +
        ")";


    updatePreview();

    expressionInput.focus();
}


// ============================================================
// BACKSPACE
// ============================================================

function backspace()
{
    clearError();


    if (
        expressionInput.value.length ===
        0
    )
    {
        return;
    }


    expressionInput.value =
        expressionInput.value.slice(
            0,
            -1
        );


    updatePreview();

    expressionInput.focus();
}


// ============================================================
// CLEAR
// ============================================================

function clearCalculator()
{
    expressionInput.value =
        "";


    display.textContent =
        "0";


    preview.textContent =
        "Ready";


    clearError();


    showStatus(
        "Calculator cleared."
    );


    expressionInput.focus();
}


// ============================================================
// BUSY STATE
// ============================================================

function setBusyState(isBusy)
{
    document
        .querySelectorAll(
            ".button, .memory-button"
        )
        .forEach(
            function(button)
            {
                button.disabled =
                    isBusy;
            }
        );


    modeButton.disabled =
        isBusy;


    themeButton.disabled =
        isBusy;


    expressionInput.disabled =
        isBusy;


    equalsButton.classList.toggle(
        "loading",
        isBusy
    );


    equalsButton.textContent =
        isBusy
            ? "…"
            : "=";


    if (isBusy)
    {
        statusMessage.textContent =
            "Calculating with C++...";
    }

    else if (
        statusMessage.textContent ===
        "Calculating with C++..."
    )
    {
        statusMessage.textContent =
            "";
    }
}


// ============================================================
// EVALUATE
// ============================================================

async function evaluateExpression()
{
    clearError();


    const expression =
        expressionInput.value.trim();


    if (
        expression.length ===
        0
    )
    {
        showError(
            "Enter an expression."
        );

        return;
    }


    try
    {
        setBusyState(
            true
        );


        const response =
            await fetch(
                API_URL,
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


        let data;


        try
        {
            data =
                await response.json();
        }

        catch
        {
            throw new Error(
                "The C++ server returned an invalid response."
            );
        }


        if (
            !response.ok ||
            data.error
        )
        {
            throw new Error(
                data.error ||
                "Expression evaluation failed."
            );
        }


        lastResult =
            Number(
                data.result
            );


        sessionStorage.setItem(
            STORAGE_KEYS.lastResult,
            String(
                lastResult
            )
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
        setBusyState(
            false
        );


        expressionInput.focus();
    }
}


// ============================================================
// COPY
// ============================================================

async function copyExpression()
{
    clearError();


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
            "Could not copy the expression."
        );
    }
}


async function copyResult()
{
    clearError();


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
            "Could not copy the result."
        );
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
                mode,

            timestamp:
                Date.now()
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


    const count =
        calculationHistory.length;


    historyCount.textContent =
        count === 1
            ? "1 calculation"
            : `${count} calculations`;


    if (count === 0)
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


            button.type =
                "button";


            button.className =
                "history-item";


            const expressionElement =
                document.createElement(
                    "span"
                );


            expressionElement.className =
                "history-expression";


            expressionElement.textContent =
                item.expression;


            const resultRow =
                document.createElement(
                    "span"
                );


            resultRow.className =
                "history-result-row";


            const resultElement =
                document.createElement(
                    "span"
                );


            resultElement.className =
                "history-result";


            resultElement.textContent =
                `= ${formatNumber(item.result)}`;


            const modeElement =
                document.createElement(
                    "span"
                );


            modeElement.className =
                "history-mode";


            modeElement.textContent =
                item.mode;


            resultRow.appendChild(
                resultElement
            );


            resultRow.appendChild(
                modeElement
            );


            button.appendChild(
                expressionElement
            );


            button.appendChild(
                resultRow
            );


            button.addEventListener(
                "click",

                function()
                {
                    reuseHistoryItem(
                        item
                    );
                }
            );


            historyList.appendChild(
                button
            );
        }
    );
}


function reuseHistoryItem(item)
{
    clearError();


    expressionInput.value =
        item.expression;


    angleMode =
        item.mode;


    sessionStorage.setItem(
        STORAGE_KEYS.angleMode,
        angleMode
    );


    updateAngleModeInterface();

    updatePreview();


    display.textContent =
        formatNumber(
            item.result
        );


    lastResult =
        item.result;


    sessionStorage.setItem(
        STORAGE_KEYS.lastResult,
        String(
            lastResult
        )
    );


    expressionInput.focus();
}


function clearHistory()
{
    calculationHistory =
        [];


    sessionStorage.removeItem(
        STORAGE_KEYS.history
    );


    renderHistory();


    showStatus(
        "History cleared."
    );


    expressionInput.focus();
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
// INPUT EVENTS
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


        else if (
            event.key ===
            "Escape"
        )
        {
            clearCalculator();
        }
    }
);


// ============================================================
// GLOBAL KEYBOARD
// ============================================================

document.addEventListener(
    "keydown",

    async function(event)
    {
        if (
            document.activeElement ===
            expressionInput
        )
        {
            return;
        }


        if (
            event.key >= "0" &&
            event.key <= "9"
        )
        {
            appendNumber(
                event.key
            );

            return;
        }


        if (
            event.key === "+" ||
            event.key === "-" ||
            event.key === "*" ||
            event.key === "/" ||
            event.key === "^" ||
            event.key === "(" ||
            event.key === ")" ||
            event.key === "!"
        )
        {
            appendToken(
                event.key
            );

            return;
        }


        if (
            event.key ===
            "."
        )
        {
            appendDecimal();

            return;
        }


        if (
            event.key ===
            "Enter"
        )
        {
            event.preventDefault();


            await evaluateExpression();

            return;
        }


        if (
            event.key ===
            "Backspace"
        )
        {
            event.preventDefault();


            backspace();

            return;
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
// INITIALIZE
// ============================================================

applyTheme();

updateAngleModeInterface();

updateMemoryIndicator();

renderHistory();

updatePreview();


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


expressionInput.focus();
