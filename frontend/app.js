const API_URL =
    "/api/evaluate";


let lastResult =
    null;


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


const expressionInput =
    document.getElementById(
        "expressionInput"
    );


// ============================================================
// HELPERS
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


function updatePreview()
{
    const expression =
        expressionInput.value;


    preview.textContent =
        expression.length > 0
            ? expression
            : "Ready";
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


// ============================================================
// CONSTANT
// ============================================================

function appendConstant(constant)
{
    appendToken(
        constant
    );
}


// ============================================================
// FUNCTION
// ============================================================

function appendFunction(functionName)
{
    appendToken(
        functionName + "("
    );
}


// ============================================================
// POWER SHORTCUTS
// ============================================================

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


// ============================================================
// FACTORIAL
// ============================================================

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


    lastResult =
        null;


    clearError();
}


// ============================================================
// BUSY
// ============================================================

function setBusyState(isBusy)
{
    document
        .querySelectorAll(
            ".button"
        )
        .forEach(
            function(button)
            {
                button.disabled =
                    isBusy;
            }
        );


    expressionInput.disabled =
        isBusy;
}


// ============================================================
// EVALUATE
// ============================================================

async function evaluateExpression()
{
    clearError();


    const expression =
        expressionInput.value.trim();


    if (expression.length === 0)
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
                                    expression
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
            data.result;


        display.textContent =
            formatNumber(
                data.result
            );


        preview.textContent =
            expression +
            " =";
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
// FORMAT NUMBER
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
// TEXT INPUT SYNC
// ============================================================

expressionInput.addEventListener(
    "input",

    function()
    {
        clearError();

        updatePreview();
    }
);


// ============================================================
// KEYBOARD
// ============================================================

expressionInput.addEventListener(
    "keydown",

    async function(event)
    {
        if (event.key === "Enter")
        {
            event.preventDefault();

            await evaluateExpression();
        }


        else if (event.key === "Escape")
        {
            clearCalculator();
        }
    }
);


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


        if (event.key === ".")
        {
            appendDecimal();

            return;
        }


        if (event.key === "Enter")
        {
            event.preventDefault();

            await evaluateExpression();

            return;
        }


        if (event.key === "Backspace")
        {
            event.preventDefault();

            backspace();

            return;
        }


        if (event.key === "Escape")
        {
            clearCalculator();
        }
    }
);


// ============================================================
// INITIALIZE
// ============================================================

updatePreview();

expressionInput.focus();
