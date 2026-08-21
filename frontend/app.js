async function calculate(operation) {
    const valueInput = document.getElementById("value");
    const resultElement = document.getElementById("result");
    const errorElement = document.getElementById("error");

    errorElement.textContent = "";

    const value = Number(valueInput.value);

    if (valueInput.value.trim() === "" || Number.isNaN(value)) {
        errorElement.textContent = "Please enter a valid number.";
        return;
    }

    resultElement.textContent = "Calculating...";

    try {
        const response = await fetch(
            "http://localhost:8080/api/calculate",
            {
                method: "POST",

                headers: {
                    "Content-Type": "application/json"
                },

                body: JSON.stringify({
                    operation: operation,
                    value: value
                })
            }
        );

        const data = await response.json();

        if (!response.ok || data.error) {
            resultElement.textContent = "0";
            errorElement.textContent =
                data.error || "Calculation failed.";
            return;
        }

        resultElement.textContent = data.result;
    }
    catch (error) {
        resultElement.textContent = "0";

        errorElement.textContent =
            "Cannot connect to the C++ server yet.";

        console.error("Calculator API error:", error);
    }
}
