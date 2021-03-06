package br.edu.ufam.dsverifier.domain.enums;

public enum DigitalSystemProperties {

	OVERFLOW("Overflow"), 
	LIMIT_CYCLE("Limit Cycle"), 
	ZERO_INPUT_LIMIT_CYCLE("Zero Input Limit Cycle"), 
	TIMING("Timing"), ERROR("Error"), 
	STABILITY("Stability"), 
	MINIMUM_PHASE("Minimum Phase"), 
	STABILITY_CLOSED_LOOP("Stability in Closed Loop"),
	LIMIT_CYCLE_CLOSED_LOOP("Limit Cycle in Closed Loop"),
	QUANTIZATION_ERROR_CLOSED_LOOP("Quantization in Closed Loop");

	private String name;

	private DigitalSystemProperties(String propertyName) {
		this.name = propertyName;
	}

	public String getName() {
		return name;
	}

	public void setName(String propertyName) {
		this.name = propertyName;
	}

}