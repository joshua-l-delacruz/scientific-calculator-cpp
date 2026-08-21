const API_URL =
    "http://localhost:8080/api/calculate";

let currentValue = "0";

let firstValue = null;

let pendingOperation = null;

let waitingForSecondValue = false;

const display =
    document.getElementById("display");

const expression =
    document.getElementById("expression");

const errorElement =
    document.getElementById("error");


function updateDisplay() {

    display.textContent =
        currentValue;

}


function clearError() {

    errorElement.textContent =
        "";

}


function showError(message) {

    errorElement.textContent =
        message;

}


function inputNumber(number) {

    clearError();

    if (
        currentValue === "0" ||
        waitingForSecondValue
    ) {

        currentValue =
            number;

        waitingForSecondValue =
            false;

    }

    else {

        currentValue +=
            number;

    }

    updateDisplay();

}


function inputDecimal() {

    clearError();

    if (waitingForSecondValue) {

        currentValue =
            "0.";

        waitingForSecondValue =
            false;

        updateDisplay();

        return;

    }

    if (!currentValue.includes(".")) {

        currentValue += ".";

    }

    updateDisplay();

}


function clearCalculator() {

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


function backspace() {

    clearError();

    if (waitingForSecondValue) {
        return;
    }

    if (
        currentValue.length <= 1 ||
        (
            currentValue.length === 2 &&
            currentValue.startsWith("-")
        )
    ) {

        currentValue =
            "0";

    }

    else {

        currentValue =
            currentValue.slice(
                0,
                -1
            );

    }

    updateDisplay();

}


async function toggleSign() {

    clearError();

    try {

        const data =
            await sendUnaryOperation(
                "negate"
            );

        currentValue =
            formatNumber(
                data.result
            );

        updateDisplay();

    }

    catch (error) {

        showError(
            error.message
        );

    }

}


function chooseOperation(operation) {

    clearError();

    firstValue =
        Number(currentValue);

    pendingOperation =
        operation;

    waitingForSecondValue =
        true;

    expression.textContent =
        `${formatNumber(firstValue)} ${operationSymbol(operation)}`;

}


async function calculateResult() {

    clearError();

    if (
        firstValue === null ||
        pendingOperation === null
    ) {

        return;

    }

    const secondValue =
        Number(currentValue);

    const originalFirst =
        firstValue;

    const originalOperation =
        pendingOperation;

    try {

        const response =
            await fetch(
                API_URL,
                {
                    method: "POST",

                    headers: {
                        "Content-Type":
                            "application/json"
                    },

                    body:
                        JSON.stringify({
                            operation:
                                originalOperation,

                            left:
                                originalFirst,

                            right:
                                secondValue
                        })
                }
            );

        const data =
            await response.json();

        if (
            !response.ok ||
            data.error
        ) {

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

    catch (error) {

        showError(
            error.message
        );

    }

}


async function scientific(operation) {

    clearError();

    const input =
        Number(currentValue);

    try {

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

    catch (error) {

        showError(
            error.message
        );

    }

}


async function sendUnaryOperation(operation) {

    const value =
        Number(currentValue);

    const response =
        await fetch(
            API_URL,
            {
                method: "POST",

                headers: {
                    "Content-Type":
                        "application/json"
                },

                body:
                    JSON.stringify({
                        operation:
                            operation,

                        value:
                            value
                    })
            }
        );

    const data =
        await response.json();

    if (
        !response.ok ||
        data.error
    ) {

        throw new Error(
            data.error ||
            "Calculation failed."
        );

    }

    return data;

}


function operationSymbol(operation) {

    const symbols = {

        add: "+",

        subtract: "−",

        multiply: "×",

        divide: "÷"

    };

    return symbols[operation] || "";

}


function operationLabel(operation) {

    const labels = {

        sqrt: "√",

        sin: "sin",

        cos: "cos",

        tan: "tan",

        log: "log",

        ln: "ln"

    };

    return labels[operation] || operation;

}


function formatNumber(number) {

    if (!Number.isFinite(number)) {

        return String(number);

    }

    if (
        Math.abs(number) < 1e-12
    ) {

        return "0";

    }

    return Number(
        number.toPrecision(12)
    ).toString();

}


// Keyboard support

document.addEventListener(
    "keydown",
    function (event) {

        if (
            event.key >= "0" &&
            event.key <= "9"
        ) {

            inputNumber(
                event.key
            );

        }

        else if (
            event.key === "."
        ) {

            inputDecimal();

        }

        else if (
            event.key === "+"
        ) {

            chooseOperation(
                "add"
            );

        }

        else if (
            event.key === "-"
        ) {

            chooseOperation(
                "subtract"
            );

        }

        else if (
            event.key === "*"
        ) {

            chooseOperation(
                "multiply"
            );

        }

        else if (
            event.key === "/"
        ) {

            event.preventDefault();

            chooseOperation(
                "divide"
            );

        }

        else if (
            event.key === "Enter" ||
            event.key === "="
        ) {

            calculateResult();

        }

        else if (
            event.key === "Backspace"
        ) {

            backspace();

        }

        else if (
            event.key === "Escape"
        ) {

            clearCalculator();

        }

    }
);


updateDisplay();
