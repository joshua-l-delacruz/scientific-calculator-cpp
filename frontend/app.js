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
// DOM
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
// DISPLAY
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
        const rawLength =
            currentValue
                .replace("-", "")
                .replace(".", "")
                .length;


        if (rawLength >= 16)
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
// API REQUEST
// ============================================================

async function apiRequest(payload)
{
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
                        payload
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
            "Calculation failed."
        );
    }


    return data;
}


// ============================================================
// CONSTANTS
// ============================================================

async function constantValue(operation)
{
    clearError();


    try
    {
        setBusyState(true);


        const data =
            await apiRequest(
                {
                    operation:
                        operation
                }
            );


        currentValue =
            formatNumber(
                data.result
            );


        expression.textContent =
            operation === "pi"
                ? "π"
                : "e";


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
// NEGATE
// ============================================================

async function toggleSign()
{
    clearError();


    try
    {
        setBusyState(true);


        const data =
            await apiRequest(
                {
                    operation:
                        "negate",

                    value:
                        Number(
                            currentValue
                        )
                }
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

    finally
    {
        setBusyState(false);
    }
}


// ============================================================
// CHOOSE BINARY OPERATION
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
        const successful =
            await calculateResult();


        if (!successful)
        {
            return;
        }
    }


    firstValue =
        Number(
            currentValue
        );


    pendingOperation =
        operation;


    waitingForSecondValue =
        true;


    expression.textContent =
        `${formatNumber(firstValue)} ${operationSymbol(operation)}`;
}


// ============================================================
// CALCULATE BINARY RESULT
// ============================================================

async function calculateResult()
{
    clearError();


    if (
        firstValue === null ||
        pendingOperation === null
    )
    {
        return false;
    }


    const secondValue =
        Number(
            currentValue
        );


    const originalFirst =
        firstValue;


    const originalOperation =
        pendingOperation;


    try
    {
        setBusyState(true);


        const data =
            await apiRequest(
                {
                    operation:
                        originalOperation,

                    left:
                        originalFirst,

                    right:
                        secondValue
                }
            );


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


        return true;
    }

    catch (error)
    {
        showError(
            error.message
        );


        return false;
    }

    finally
    {
        setBusyState(false);
    }
}


// ============================================================
// SCIENTIFIC UNARY OPERATION
// ============================================================

async function scientific(operation)
{
    clearError();


    const input =
        Number(
            currentValue
        );


    try
    {
        setBusyState(true);


        const data =
            await apiRequest(
                {
                    operation:
                        operation,

                    value:
                        input
                }
            );


        expression.textContent =
            scientificExpression(
                operation,
                input
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

    finally
    {
        setBusyState(false);
    }
}


// ============================================================
// SCIENTIFIC EXPRESSION LABEL
// ============================================================

function scientificExpression(
    operation,
    value
)
{
    const formatted =
        formatNumber(
            value
        );


    switch (operation)
    {
        case "sqrt":

            return `√(${formatted})`;


        case "square":

            return `${formatted}²`;


        case "cube":

            return `${formatted}³`;


        case "reciprocal":

            return `1 / ${formatted}`;


        case "percent":

            return `${formatted}%`;


        case "factorial":

            return `${formatted}!`;


        case "sin":

            return `sin(${formatted}°)`;


        case "cos":

            return `cos(${formatted}°)`;


        case "tan":

            return `tan(${formatted}°)`;


        case "log":

            return `log(${formatted})`;


        case "ln":

            return `ln(${formatted})`;


        default:

            return `${operation}(${formatted})`;
    }
}


// ============================================================
// BINARY SYMBOLS
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
            "÷",

        power:
            "^"
    };


    return symbols[operation] || "";
}


// ============================================================
// FORMAT NUMBERS
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
        // ----------------------------------------------------
        // NUMBERS
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // DECIMAL
        // ----------------------------------------------------

        if (event.key === ".")
        {
            inputDecimal();

            return;
        }


        // ----------------------------------------------------
        // ADD
        // ----------------------------------------------------

        if (event.key === "+")
        {
            await chooseOperation(
                "add"
            );

            return;
        }


        // ----------------------------------------------------
        // SUBTRACT
        // ----------------------------------------------------

        if (event.key === "-")
        {
            await chooseOperation(
                "subtract"
            );

            return;
        }


        // ----------------------------------------------------
        // MULTIPLY
        // ----------------------------------------------------

        if (event.key === "*")
        {
            await chooseOperation(
                "multiply"
            );

            return;
        }


        // ----------------------------------------------------
        // DIVIDE
        // ----------------------------------------------------

        if (event.key === "/")
        {
            event.preventDefault();


            await chooseOperation(
                "divide"
            );


            return;
        }


        // ----------------------------------------------------
        // POWER
        // ----------------------------------------------------

        if (event.key === "^")
        {
            await chooseOperation(
                "power"
            );

            return;
        }


        // ----------------------------------------------------
        // EQUALS
        // ----------------------------------------------------

        if (
            event.key === "Enter" ||
            event.key === "="
        )
        {
            event.preventDefault();


            await calculateResult();


            return;
        }


        // ----------------------------------------------------
        // BACKSPACE
        // ----------------------------------------------------

        if (event.key === "Backspace")
        {
            event.preventDefault();


            backspace();


            return;
        }


        // ----------------------------------------------------
        // CLEAR
        // ----------------------------------------------------

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
