const API_URL =
    "/api/calculate";


let currentValue =
    "0";

let firstValue =
    null;

let pendingOperation =
    null;

let waitingForSecondValue =
    false;


// ============================================================
// DOM ELEMENTS
// ============================================================

const display =
    document.getElementById(
        "display"
    );

const expression =
    document.getElementById(
        "expression"
    );

const errorElement =
    document.getElementById(
        "error"
    );


// ============================================================
// DISPLAY HELPERS
// ============================================================

function updateDisplay()
{
    display.textContent =
        currentValue;
}


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


// ============================================================
// NUMBER INPUT
// ============================================================

function inputNumber(number)
{
    clearError();


    if (
        currentValue === "0" ||
        waitingForSecondValue
    )
    {
        currentValue =
            number;

        waitingForSecondValue =
            false;
    }

    else
    {
        if (
            currentValue.replace(
                "-",
                ""
            ).length >= 16
        )
        {
            return;
        }


        currentValue +=
            number;
    }


    updateDisplay();
}


// ============================================================
// DECIMAL
// ============================================================

function inputDecimal()
{
    clearError();


    if (waitingForSecondValue)
    {
        currentValue =
            "0.";

        waitingForSecondValue =
            false;

        updateDisplay();

        return;
    }


    if (!currentValue.includes("."))
    {
        currentValue +=
            ".";
    }


    updateDisplay();
}


// ============================================================
// CLEAR
// ============================================================

function clearCalculator()
{
    currentValue =
        "0";

    firstValue =
        null;

    pendingOperation =
        null;

    waitingForSecondValue =
        false;

    expression.textContent =
        "";

    clearError();

    updateDisplay();
}


// ============================================================
// BACKSPACE
// ============================================================

function backspace()
{
    clearError();


    if (waitingForSecondValue)
    {
        return;
    }


    if (
        currentValue.length <= 1 ||
        (
            currentValue.length === 2 &&
            currentValue.startsWith("-")
        )
    )
    {
        currentValue =
            "0";
    }

    else
    {
        currentValue =
            currentValue.slice(
                0,
                -1
            );
    }


    updateDisplay();
}


// ============================================================
// CHANGE SIGN
// ============================================================

async function toggleSign()
{
    clearError();


    try
    {
        const data =
            await sendUnaryOperation(
                "negate"
            );


        currentValue =
            formatNumber(
                data.result
            );


        waitingForSecondValue =
            true;


        updateDisplay();
    }

    catch (error)
    {
        showError(
            error.message
        );
    }
}


// ============================================================
// BINARY OPERATION
// ============================================================

async function chooseOperation(operation)
{
    clearError();


    if (
        firstValue !== null &&
        pendingOperation !== null &&
        !waitingForSecondValue
    )
    {
        await calculateResult();
    }


    firstValue =
        Number(currentValue);

    pendingOperation =
        operation;

    waitingForSecondValue =
        true;


    expression.textContent =
        `${formatNumber(firstValue)} ${operationSymbol(operation)}`;
}


// ============================================================
// RESULT
// ============================================================

async function calculateResult()
{
    clearError();


    if (
        firstValue === null ||
        pendingOperation === null
    )
    {
        return;
    }


    const secondValue =
        Number(currentValue);

    const originalFirst =
        firstValue;

    const originalOperation =
        pendingOperation;


    try
    {
        setBusyState(true);


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
                                operation:
                                    originalOperation,

                                left:
                                    originalFirst,

                                right:
                                    secondValue
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


        expression.textContent =
            `${formatNumber(originalFirst)} ${operationSymbol(originalOperation)} ${formatNumber(secondValue)} =`;


        currentValue =
            formatNumber(
                data.result
            );


        firstValue =
            null;

        pendingOperation =
            null;

        waitingForSecondValue =
            true;


        updateDisplay();
    }

    catch (error)
    {
        showError(
            error.message
        );
    }

    finally
    {
        setBusyState(false);
    }
}


// ============================================================
// SCIENTIFIC OPERATIONS
// ============================================================

async function scientific(operation)
{
    clearError();


    const input =
        Number(currentValue);


    try
    {
        setBusyState(true);


        const data =
            await sendUnaryOperation(
                operation
            );


        expression.textContent =
            `${operationLabel(operation)}(${formatNumber(input)})`;


        currentValue =
            formatNumber(
                data.result
            );


        waitingForSecondValue =
            true;


        updateDisplay();
    }

    catch (error)
    {
        showError(
            error.message
        );
    }

    finally
    {
        setBusyState(false);
    }
}


// ============================================================
// API
// ============================================================

async function sendUnaryOperation(operation)
{
    const value =
        Number(currentValue);


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
                            operation:
                                operation,

                            value:
                                value
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


    return data;
}


// ============================================================
// BUSY STATE
// ============================================================

function setBusyState(isBusy)
{
    const buttons =
        document.querySelectorAll(
            ".button"
        );


    buttons.forEach(
        function(button)
        {
            button.disabled =
                isBusy;
        }
    );
}


// ============================================================
// SYMBOLS
// ============================================================

function operationSymbol(operation)
{
    const symbols =
    {
        add:
            "+",

        subtract:
            "−",

        multiply:
            "×",

        divide:
            "÷"
    };


    return symbols[operation] || "";
}


function operationLabel(operation)
{
    const labels =
    {
        sqrt:
            "√",

        sin:
            "sin",

        cos:
            "cos",

        tan:
            "tan",

        log:
            "log",

        ln:
            "ln"
    };


    return labels[operation] || operation;
}


// ============================================================
// NUMBER FORMATTING
// ============================================================

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
        Math.abs(number) >= 1e12 ||
        Math.abs(number) < 1e-9
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
// KEYBOARD SUPPORT
// ============================================================

document.addEventListener(
    "keydown",

    async function(event)
    {
        if (
            event.key >= "0" &&
            event.key <= "9"
        )
        {
            inputNumber(
                event.key
            );

            return;
        }


        if (event.key === ".")
        {
            inputDecimal();

            return;
        }


        if (event.key === "+")
        {
            await chooseOperation(
                "add"
            );

            return;
        }


        if (event.key === "-")
        {
            await chooseOperation(
                "subtract"
            );

            return;
        }


        if (event.key === "*")
        {
            await chooseOperation(
                "multiply"
            );

            return;
        }


        if (event.key === "/")
        {
            event.preventDefault();

            await chooseOperation(
                "divide"
            );

            return;
        }


        if (
            event.key === "Enter" ||
            event.key === "="
        )
        {
            event.preventDefault();

            await calculateResult();

            return;
        }


        if (event.key === "Backspace")
        {
            event.preventDefault();

            backspace();

            return;
        }


        if (
            event.key === "Escape" ||
            event.key === "Delete"
        )
        {
            clearCalculator();
        }
    }
);


// ============================================================
// INITIALIZE
// ============================================================

updateDisplay();
